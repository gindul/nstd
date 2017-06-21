/*
MIT License
Copyright (c) 2017 Arlen Keshabyan (arlen.albert@gmail.com)
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <string>
#include <tuple>
#include "default_random_provider.hpp"
#include "relinx.hpp"
#include "sqlite.hpp"

int main()
{
    nstd::sqlite::database db{ ":memory:" };

    db << "create table if not exists example(id int primary key, name text, password text)";
    db << "delete from example";

    {
        nstd::db::scoped_transaction tr(db);

        nstd::relinx::range(1, 100)->for_each([&db](auto &&idx)
        {
            auto name { std::to_string(nstd::default_random_provider<>()()) };
            auto password { std::to_string(nstd::default_random_provider<>()()) };

            db << "insert into example(id, name, password) values (?, ?, ?)" << idx << name << password;
        });
    }

    using example_rows = nstd::db::tuple_records<int, std::string, std::string>;

    example_rows records { 50 };

    db << "select id, name, password from example where id between 30 and 80" >> records;

    nstd::relinx::from(std::data(records))->for_each([](auto &&row)
    {
        auto &[id, name, password] = row;

        std::cout << "id: " << id << ";\tname: " << name << ";\tpassword: " << password << std::endl;
    });

    db << "select name, password from example order by name desc limit 20" >> [](std::string name, std::string password)
    {
        std::cout << "name: " << name << ";\tpassword: " << password << std::endl;
    };

    std::cout << "exiting..." << std::endl;

    return 0;
}