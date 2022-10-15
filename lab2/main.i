# 1 "main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "main.c"
int main()
{
 int i, n, f;
 cin >> n;
 i = 2;
 f = 1;
 while (i <= n)
 {
  f = f * i;
  i = i + 1;
 }
 cout << f << endl;
}
