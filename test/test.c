int rand_test_main(void);
int ju_test_main(void);
int regex_test_main(void);
int path_test_main(void);
int rp_test_main(void);

int exit_code;
#define EXIT(x) exit_code = x; if (exit_code) return exit_code;

int main(void) {
    EXIT(rand_test_main())
    EXIT(ju_test_main())
    EXIT(regex_test_main())
    EXIT(path_test_main())
    EXIT(rp_test_main())
    return 0;
}
