#include "mysql_table_update.h"

#include "libserver/log4_help.h"
#include "libserver/util_string.h"
#include "libserver/component_help.h"

#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <thread>

MysqlTableUpdate::MysqlTableUpdate()
{
    // ע����º��������±�ִ�У�ע��˳��
    _update_func.push_back(BindFunP0(this, &MysqlTableUpdate::Update00)); 
}

MysqlTableUpdate::~MysqlTableUpdate()
{
    Disconnect();
}

void MysqlTableUpdate::Check()
{
    if (!ConnectInit())
        return;

    auto pYaml = ComponentHelp::GetYaml();
    auto pDbMgrCfig = dynamic_cast<DBMgrConfig*>(pYaml->GetConfig(APP_DB_MGR));
    _pDbCfg = pDbMgrCfig->GetDBConfig(DBMgrConfig::DBTypeMysql);
    if (_pDbCfg == nullptr)
    {
        LOG_ERROR("Init failed. get mysql config is failed.");
        return;
    }

    LOG_DEBUG("Mysql update connect. " << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port << " starting... id:" << std::this_thread::get_id());

    if (mysql_real_connect(_pMysql, _pDbCfg->Ip.c_str(), _pDbCfg->User.c_str(), _pDbCfg->Password.c_str(), nullptr, _pDbCfg->Port, nullptr, CLIENT_FOUND_ROWS))
    {
        mysql_select_db(_pMysql, _pDbCfg->DatabaseName.c_str());
    }

    int mysqlerrno = CheckMysqlError();
    if (mysqlerrno == ER_BAD_DB_ERROR)
    {
        LOG_DEBUG("Mysql. try create database:" << _pDbCfg->DatabaseName.c_str());

        // 1049: Unknown database�� û���ҵ����ݿ⣬���½�һ��
        if (!CreateDatabaseIfNotExist())
        {
            Disconnect();
            return;
        }

        mysql_select_db(_pMysql, _pDbCfg->DatabaseName.c_str());
        mysqlerrno = CheckMysqlError();
    }

    if (mysqlerrno > 0)
    {
        Disconnect();
        return;
    }

    // ���汾���Զ�����
    if (!UpdateToVersion())
    {
        LOG_ERROR("!!!Failed. Mysql update. UpdateToVersion");
        return;
    }

    mysql_ping(_pMysql);
    mysqlerrno = CheckMysqlError();
    if (mysqlerrno > 0)
    {
        Disconnect();
        return;
    }

    LOG_DEBUG("Mysql Update successfully! addr:" << _pDbCfg->Ip.c_str() << ":" << _pDbCfg->Port);
}

bool MysqlTableUpdate::CreateDatabaseIfNotExist()
{
    // �Ƿ�������ݿ⣬��������ڣ��򴴽�
    std::string querycmd = strutil::format("CREATE DATABASE IF NOT EXISTS %s;", _pDbCfg->DatabaseName.c_str());

    my_ulonglong affected_rows;
    if (!Query(querycmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist. cmd:" << querycmd.c_str());
        return false;
    }

    // �����ϵ�DB֮��ѡ��ָ�������ݿ�
    if (mysql_select_db(_pMysql, _pDbCfg->DatabaseName.c_str()) != 0) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist: mysql_select_db:" << LOG4CPLUS_STRING_TO_TSTRING(_pDbCfg->DatabaseName));
        return false;
    }

    // ��������Ϊ���ַ�������yaml�������ж�ȡ������Ҫ���ַ���
    if (mysql_set_character_set(_pMysql, _pDbCfg->CharacterSet.c_str()) != 0) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist: Could not set client connection character set to " << LOG4CPLUS_STRING_TO_TSTRING(_pDbCfg->CharacterSet));
        return false;
    }

    // ���������ݿ��Сд���У������ļ���Ϊ utf8_general_ci
    querycmd = strutil::format("ALTER DATABASE CHARACTER SET %s COLLATE %s", _pDbCfg->CharacterSet.c_str(), _pDbCfg->Collation.c_str());
    if (!Query(querycmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateDatabaseIfNotExist. cmd:" << LOG4CPLUS_STRING_TO_TSTRING(querycmd.c_str()));
        return false;
    }

    // ����һ��version ��ʹ���� InnoDB ��ʽ
    std::string create_version =
        "CREATE TABLE IF NOT EXISTS `version` (" \
        "`version` int(11) NOT NULL," \
        "PRIMARY KEY (`version`)" \
        ") ENGINE=%s DEFAULT CHARSET=%s;";

    std::string cmd = strutil::format(create_version.c_str(), "InnoDB", _pDbCfg->CharacterSet.c_str());
    if (!Query(cmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateTable. " << LOG4CPLUS_STRING_TO_TSTRING(cmd));
        return false;
    }

    // ����һ�� player ��
    std::string create_player =
        "CREATE TABLE IF NOT EXISTS `player` (" \
        "`sn` bigint(20) NOT NULL," \
        "`name` char(32) NOT NULL," \
        "`account` char(64) NOT NULL," \
        "`base` blob," \
        "`item` blob,"  \
        "`misc` blob,"  \
        "`savetime` datetime default NULL," \
        "`createtime` datetime default NULL," \
        "PRIMARY KEY  (`sn`),"  \
        "UNIQUE KEY `NAME` (`name`),"  \
        "KEY `ACCOUNT` (`account`)"  \
        ") ENGINE=%s DEFAULT CHARSET=%s;";

    cmd = strutil::format(create_player.c_str(), "InnoDB", _pDbCfg->CharacterSet.c_str());
    if (!Query(cmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateTable" << LOG4CPLUS_STRING_TO_TSTRING(cmd));
        return false;
    }

    // ��󴴽����ˣ��޸� version�����version �ֶΣ���Ϊ��ʼ��0�Ű汾
    cmd = "insert into `version` VALUES ('0')";
    if (!Query(cmd.c_str(), affected_rows)) {
        LOG_ERROR("!!! Failed. MysqlConnector::CreateTable." << LOG4CPLUS_STRING_TO_TSTRING(cmd));
        return false;
    }

    return true;
}

bool MysqlTableUpdate::UpdateToVersion()
{
    my_ulonglong affected_rows;
    std::string sql = "select version from `version`";
    if (!Query(sql.c_str(), affected_rows))
        return false;

    MYSQL_ROW row = Fetch();
    if (row == nullptr)
        return false;

    int version = GetInt(row, 0);
    if (version == _version)
        return true;

    // ���DB�汾��ƥ�䣬����DB
    for (int i = version + 1; i <= _version; i++) {
        if (_update_func[i] == nullptr)
            continue;

        if (!_update_func[i]()) {
            LOG_ERROR("UpdateToVersion failed!!!!!, version=" << i);
            return false;
        }

        LOG_INFO("update db to version:" << i);

        // �ɹ�֮�󣬸���DB��version
        std::string cmd = strutil::format("update `version` set version = %d", i);
        if (!Query(cmd.c_str(), affected_rows)) {
            LOG_ERROR("UpdateToVersion failed!!!!!, change version failed. version=" << i);
            return false;
        }
    }

    return true;
}

bool MysqlTableUpdate::Update00()
{
    return true;
}

//bool MysqlTableUpdate::Update01()
//{
//    std::string sql = "ALTER TABLE `player` ADD COLUMN `testa`  blob NULL AFTER `misc`;";
//    my_ulonglong affected_rows;
//    if (!Query(sql.c_str(), affected_rows)) {
//        LOG_ERROR("!!! Failed. MysqlTableUpdate::Update01. " << sql.c_str());
//        return false;
//    }
//
//    return true;
//}
