#pragma once

// redis �ؽ���

// ��¼ Token
#define RedisKeyAccountTokey  "engine::token::"
#define RedisKeyAccountTokeyTimeout  2 * 60

// ��ɫ��¼
#define RedisKeyAccountOnlineLogin  "engine::online::login::"
#define RedisKeyAccountOnlineLoginTimeout  6 * 60

// ��ɫ����
#define RedisKeyAccountOnlineGame  "engine::online::game::"
#define RedisKeyAccountOnlineGameTimeout  6 * 60
