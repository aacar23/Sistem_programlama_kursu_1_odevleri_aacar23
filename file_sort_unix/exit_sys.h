#if !defined EXIT_SYS_H
#define EXIT_SYS_H

void prepend_message_to_error_str(const char *message);
void set_error_str_errno(const char *additional_message);
void set_error_str_custom(const char *error_message);
void exit_sys(void);

#endif