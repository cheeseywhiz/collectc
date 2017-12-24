extern int randtest(void);
extern int ju_test_main(void);
extern int regex_test_main(void);
extern int path_test_main(void);

int exit_code;
#define EXIT(x) exit_code = x; if (exit_code) return exit_code;

int main(void) {
    EXIT(randtest())
    EXIT(ju_test_main())
    EXIT(regex_test_main())
    EXIT(path_test_main())
    return 0;
}
