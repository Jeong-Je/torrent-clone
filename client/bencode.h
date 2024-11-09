void bencode_dict_start(FILE *fp);
void bencode_dict_end(FILE *fp);
void bencode_list_start(FILE *fp);
void bencode_list_end(FILE *fp);
void bencode_int(FILE *fp, int value);
void bencode_string(FILE *fp, char *str);
