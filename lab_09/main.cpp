#include <iostream>
#include <fstream>
#include <string>
#include <tuple>


#include <boost/program_options.hpp>

#include "c_requester.h"
#include "c_queue.h"
#include "c_parser.h"

void downloader_thread_function(c_queue<std::tuple<int, std::string>> &urls,
                                c_queue<std::tuple<int, std::string>> &contents,
                                int max_depth)
{
    while(1)
    {
        auto urls_item = urls.pop();
        auto depth = std::get<0>(urls_item);
        auto current_url = std::get<1>(urls_item);
        if(current_url.substr(0, 2) == "//")
        {
            current_url = "https:" + current_url + "/";
        }

        if(depth > max_depth)
        {
            continue;
        }

        try
        {
            c_requester requester;
            auto content = requester.do_request(current_url);
            if(content.empty())
            {
                std::cerr << "Error! Empty content. url=" << current_url << std::endl;
                continue;
            }
            contents.push(std::make_tuple(depth, content));
        }
        catch (std::exception &ex)
        {
            std::cerr<< std::endl << std::endl
                     << "-------------------------------------------------------------------------------------"
                     << "Error! We have exception[downloader_thread_function]: " << ex.what()
                     << "-------------------------------------------------------------------------------------"
                     << std::endl << std::endl;
        }
    }
}

void parser_thread_function(c_queue<std::tuple<int, std::string>> &contents,
                            c_queue<std::tuple<int, std::string>> &urls,
                            c_queue<std::string> &images,
                            int max_depth)
{
    while(1)
    {
        auto contents_item = contents.pop();
        auto current_depth = std::get<0>(contents_item);
        auto content_body = std::get<1>(contents_item);

        if(current_depth > max_depth)
            continue;
        try
        {
            c_parser parser(content_body);

            std::vector<std::string> list_links;
            parser.get_links(list_links);
            for(auto &link_item: list_links)
            {
                urls.push(std::make_tuple(current_depth+1, link_item));
            }

            std::vector<std::string> list_images;
            parser.get_images(list_images);
            for(auto &image_item: list_images)
            {
                images.push(image_item);
            }
        }
        catch (std::exception &ex)
        {
            std::cerr<< std::endl << std::endl
                     << "-------------------------------------------------------------------------------------"
                     << "Error! We have exception[parser_thread_function]: " << ex.what()
                     << "-------------------------------------------------------------------------------------"
                     << std::endl << std::endl;
        }
    }
}

void writer_thread_function(c_queue<std::string> &images, std::string &filename)
{
    std::ofstream out(filename.c_str());
    while(1)
    {
        auto images_item = images.pop();
        out << images_item << std::endl;
    }
    out.close();
}

using namespace boost::program_options;

int main(int argc, char* argv[])
{
    try
    {
        std::system("chcp 1251");



        std::string url;
        int max_depth = 1;
        int network_threads = 1;
        int parser_threads = 1;
        std::string output("images.txt");

        options_description desc("Program options");
        desc.add_options()
        ("help,h", "print usage message")
        ("output,o", value<std::string>(), "pathname for output")
        ("depth,d", value<int>(), "max depth")
        ("network_threads,n", value<int>(), "count of download threads")
        ("parser_threads,p", value<int>(), "count of parse threads")
        ("url,u",value<std::string>(), "url")
        ;

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count("output"))
        {
            output = vm["output"].as<std::string>();
            std::cout << "output " << output << std::endl;
        }

        if (vm.count("url"))
        {
            url = vm["url"].as<std::string>();
            std::cout << "url " << url << std::endl;
        }

        if (vm.count("depth"))
        {
            max_depth = vm["depth"].as<int>();
            std::cout << "url " << max_depth << std::endl;
        }

        if (vm.count("network_threads"))
        {
            network_threads = vm["network_threads"].as<int>();
            std::cout << "network_threads " << network_threads << std::endl;
        }

        if (vm.count("parser_threads"))
        {
            parser_threads = vm["parser_threads"].as<int>();
            std::cout << "parser_threads " << parser_threads << std::endl;
        }

        c_queue<std::tuple<int, std::string>> contents;
        c_queue<std::tuple<int, std::string>> urls;
        c_queue<std::string> images;

        urls.push(std::make_tuple(0, url)); //"https://yandex.ru/"



        std::vector<std::thread> downloaders;
        for (int i = 0; i < network_threads; ++i)
        {
            std::thread downloader(std::bind(&downloader_thread_function, std::ref(urls), std::ref(contents), max_depth));
            downloaders.push_back(std::move(downloader));
        }

        std::vector<std::thread> parsers;
        for (int i = 0; i < parser_threads; ++i)
        {
            std::thread parser(std::bind(&parser_thread_function, std::ref(contents), std::ref(urls), std::ref(images), max_depth));
            parsers.push_back(std::move(parser));
        }

        std::thread writer(std::bind(&writer_thread_function, std::ref(images), std::ref(output)));

        std::cin.get();

        for (auto & downloader: downloaders)
        {
            if(downloader.joinable())
                downloader.join();
        }

        for (auto & parser: parsers)
        {
            if(parser.joinable())
                parser.join();
        }

        if(writer.joinable())
            writer.join();

    }
    catch(std::exception const& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return EXIT_SUCCESS;
}
