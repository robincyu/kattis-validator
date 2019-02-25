#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

void _assert(bool expr, const std::string t)
{
    if (!expr)
    {
        std::cerr << "Assert failed!\n";
        std::cerr << t << '\n';
        exit(1);
        //abort();
    }
}

void _assert(bool expr, int line, int pos, const std::string t)
{
    if (!expr)
    {
        std::cerr << "Assert failed at line " << line << " pos " << pos << "!\n";
        std::cerr << t << '\n';
        exit(1);
        //abort();
    }
}

std::string to_string(long long int num)
{
    bool is_neg = false;
    std::string str = "";
    if (num == 0)
        return "0";
    else if (num < 0)
    {
        is_neg = true;
        num *= -1;
    }
    while (num > 0)
    {
        str += '0' + (num % 10);
        num /= 10;
    }
    if (is_neg)
        str += '-';
    std::reverse(str.begin(), str.end());
    return str;
}

class stream
{
    private:
        int line_num;
        int char_num;
        int rd;
        bool fail;
        std::string str;
        std::string gulp(std::istream &in)
        {
            std::string ret;
            char buffer[4096];
            while (in.read(buffer, sizeof(buffer)))
                ret.append(buffer, sizeof(buffer));
            ret.append(buffer, in.gcount());
            return ret;
        }
        bool stream_empty()
        {
            return (rd == (int)str.length());
        }
        bool is_digit(char c)
        {
            return ('0' <= c && c <= '9');
        }
        bool is_lowercase(char c)
        {
            return ('a' <= c && c <= 'z');
        }
        bool is_uppercase(char c)
        {
            return ('A' <= c && c <= 'Z');
        }
        bool is_space(char c)
        {
            return (c == ' ');
        }
        bool is_newline(char c)
        {
            return (c == '\n');
        }
        std::string printable(char c)
        {
            std::string tmp;
            if (c == '\n')
                tmp = "\\n";
            else
                tmp = c;
            return tmp;
        }
    public:
        stream(std::istream &in)
        {
            rd = 0;
            line_num = 1;
            char_num = 1;
            fail = false;
            str = gulp(in);
        }
        bool done()
        {
            _assert(!fail && stream_empty(), line_num, char_num, "Expected EOF, but stream is not empty.");
            return !fail && stream_empty();
        }
        void read_char(char c)
        {
            std::string expected = printable(c);
            if (stream_empty() || fail) //never read from an invalid stream
            {
                fail = true;
                _assert(false, line_num, char_num, "Expected '" + expected + "', but stream is empty.");
            }
            else if (str.at(rd) != c)
            {
                std::string found = printable(str.at(rd));
                fail = true;
                _assert(false, line_num, char_num, "Expected '" + expected + "', but found '" + found + "'.");
            }
            else
                rd++;
            if (c == '\n')
            {
                line_num++;
                char_num = 1;
            }
            else
                char_num++;
        }
        void read_space()
        {
            read_char(' ');
        }
        void read_newline()
        {
            read_char('\n');
        }
        std::string read_word()
        {
            std::string pr = "";
            if (stream_empty() || fail) //never read from an invalid stream
            {
                fail = true;
                _assert(false, line_num, char_num, "Expected word, but stream is empty.");
            }
            else
            {
                for (int t_rd = rd; t_rd < (int)str.length(); t_rd++)
                {
                    if (is_space(str.at(t_rd)) || is_newline(str.at(t_rd)))
                        break;
                    else
                        pr += str.at(t_rd);
                }
                if (pr.empty())
                {
                    fail = true;
                    if (is_space(str.at(rd)))
                        _assert(false, line_num, char_num, "Expected word, but found ' '.");
                    else
                        _assert(false, line_num, char_num, "Expected word, but found '\\n'.");
                }
            }
            rd += pr.length();
            char_num += pr.length();
            return pr;
        }
        long long int read_int(long long int l, long long int r)
        {
            long long int val = 0;
            std::string num = "";
            std::string pr = "";
            if (stream_empty() || fail) //never read from an invalid stream
            {
                fail = true;
                _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but stream is empty.");
            }
            else
            {
                for (int t_rd = rd; t_rd < (int)str.length(); t_rd++)
                {
                    if (is_space(str.at(t_rd)) || is_newline(str.at(t_rd)))
                        break;
                    else
                        pr += str.at(t_rd);
                }
                if (pr.empty())
                {
                    fail = true;
                    if (is_space(str.at(rd)))
                        _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found ' '.");
                    else
                        _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found '\\n'.");
                }
                bool is_neg = false;
                for (; rd < (int)str.length(); rd++)
                {
                    if (str.at(rd) == '-')
                    {
                        //there's only 1 place this is okay, which is at the start
                        if (num == "" && !is_neg)
                            is_neg = true;
                        else
                        {
                            fail = true;
                            _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found non-integer word \"" + pr + "\".");
                        }
                    }
                    else if (is_digit(str.at(rd)))
                        num += str.at(rd);
                    else if (is_space(str.at(rd)) || is_newline(str.at(rd)))
                        break;
                    else
                    {
                        fail = true;
                        _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found non-integer word \"" + pr + "\".");
                    }
                }
                if (!fail)
                {
                    if (num == "")
                    {
                        fail = true;
                        _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found non-integer word \"" + pr + "\".");
                    }
                    else
                    {
                        if (num.at(0) == '0' && num.length() > 1)
                        {
                            fail = true;
                            _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found non-integer word \"" + pr + "\".");
                        }
                        else if (num.at(0) == '0' && is_neg)
                        {
                            fail = true;
                            _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found non-integer word \"" + pr + "\".");
                        }
                        else
                        {
                            std::string limit = "9223372036854775807";
                            if (num.length() > limit.length())
                            {
                                fail = true;
                                _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found \"" + pr + "\".");
                            }
                            else if (num.length() == limit.length())
                            {
                                for (int i = 0; i < (int)num.length(); i++)
                                {
                                    if (num.at(i) < limit.at(i))
                                        break;
                                    else if (num.at(i) > limit.at(i))
                                    {
                                        fail = true;
                                        _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found \"" + pr + "\".");
                                    }
                                }
                            }
                            if (!fail)
                            {
                                long long int num_int;
                                std::stringstream ss;
                                ss << num;
                                ss >> num_int;
                                ss.clear();
                                ss.str("");
                                num_int *= (is_neg ? -1 : 1);
                                if (l <= num_int && num_int <= r)
                                {
                                    val = num_int;
                                }
                                else
                                {
                                    fail = true;
                                    _assert(false, line_num, char_num, "Expected integer in range [" + to_string(l) + ", " + to_string(r) + "], but found \"" + pr + "\".");
                                }
                            }
                        }
                    }
                }
            }
            char_num += pr.length();
            return val;
        }
};

/*
Example:

The input consists of two integers, a and b, separated by a space.
(1 <= a, b <= 100; a + b <= 100)

*/

int main()
{
    stream st = stream(std::cin);
    int a = st.read_int(1, 100);
    st.read_space();
    int b = st.read_int(1, 100);
    _assert(a + b <= 100, "a + b must be <= 100.");
    st.read_newline();
    st.done();
    exit(42);
}
