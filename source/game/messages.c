internal b32 GetFirstFreeMessage(Message** msg_ptr) {
    Message* msg = NULL;
    for (u32 i = 0; i < MAX_MESSAGES; ++i) {
        Message* result = &game->messages[i];
        if (!result->active) {
            msg = result;
            break;
        }
    }

    *msg_ptr = msg;
    return msg != NULL;
}

internal b32 GetNextActiveMessage(Message** msg_ptr) {
    Message* msg = *msg_ptr;

    u32 start_index = 0;
    if (msg != NULL) {
        start_index = (u32) (msg - game->messages + 1);
    }

    msg = NULL;
    for (u32 i = start_index; i < MAX_MESSAGES; ++i) {
        Message* result = &game->messages[i];
        if (result->active) {
            msg = result;
            break;
        }
    }

    *msg_ptr = msg;
    return msg != NULL;
}

internal void PushMessage(f32 max_age, const char* format, ...) {
    Message* free = NULL;
    if (!GetFirstFreeMessage(&free)) {
        return;
    }

    va_list args;
    va_start(args, format);

    {
        // TODO(geni): Add ... to end if forced to truncate
        free->size             = stbsp_vsnprintf((char*) free->data, MAX_MESSAGE_SIZE, format, args);
        free->data[free->size] = '\0';
        free->max_age          = max_age;
        free->active           = 1;
    }

    va_end(args);
}

internal void UpdateMessages(f32 delta) {
    for (u32 i = 0; i < MAX_MESSAGES; ++i) {
        Message* msg = &game->messages[i];
        if (msg->active) {
            msg->age += delta;
            if (msg->age > msg->max_age) {
                *msg = (Message){0};
            }
        }
    }
}

internal void ClearMessages(void) {
    for (u32 i = 0; i < MAX_MESSAGES; ++i) {
        Message* msg = &game->messages[i];
        msg->active  = 0;
    }
}
