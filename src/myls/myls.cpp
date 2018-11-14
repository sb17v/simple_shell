#include "myls.h"

std::string get_grpname(const struct stat status)
{
    struct group *group;
    unsigned int grp_id = status.st_gid;

    group = getgrgid(grp_id);

    if (group != NULL)
    {
        return std::string(group->gr_name);
    }
    else
    {
        char gid[80];
        sprintf(gid, "%d", grp_id);
        return std::string(gid);
    }
}

std::string get_usrname(const struct stat status)
{
    struct passwd *passwrd;
    unsigned int usr_id = status.st_uid;

    passwrd = getpwuid(usr_id);

    if (passwrd != NULL)
    {
        return std::string(passwrd->pw_name);
    }
    else
    {
        char uid[80];
        sprintf(uid, "%d", usr_id);
        return std::string(uid);
    }
}

std::string get_permission(const struct stat status)
{
    std::string permission = "";

    if (S_ISLNK(status.st_mode))
    {
        permission += "l";
    }
    else if (S_ISREG(status.st_mode))
    {
        permission += "-";
    }
    else if (S_ISDIR(status.st_mode))
    {
        permission += "d";
    }

    if (status.st_mode & S_IRUSR)
    {
        permission += "r";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IWUSR)
    {
        permission += "w";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IXUSR)
    {
        permission += "x";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IRGRP)
    {
        permission += "r";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IWGRP)
    {
        permission += "w";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IXGRP)
    {
        permission += "x";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IROTH)
    {
        permission += "r";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IWOTH)
    {
        permission += "w";
    }
    else
    {
        permission += "-";
    }
    if (status.st_mode & S_IXOTH)
    {
        permission += "x";
    }
    else
    {
        permission += "-";
    }

    return permission;
}

std::string get_formatted_date_time(const struct stat status)
{
    char date_time[20];
    strftime(date_time, 20, "%b %d %H:%M", localtime(&status.st_ctime));
    return std::string(date_time);
}

void print_output(const struct stat status, std::string file)
{
    std::string real_path = "";
    std::string real_path_output = "";
    int entries = 1;

    if (S_ISLNK(status.st_mode))
    {
        char buffer[PATH_MAX + 1];
        char *path = realpath(file.c_str(), buffer);
        real_path = std::string(path);
    }
    else if (S_ISDIR(status.st_mode))
    {
        struct dirent *dir_entry;
        DIR *dir = opendir(file.c_str());
        if (dir)
        {
            while ((dir_entry = readdir(dir)) != NULL)
            {
                std::string dir_filename(dir_entry->d_name);
                if (!dir_filename.empty() && dir_filename.at(0) == '.')
                {
                    continue;
                }
#ifdef _DIRENT_HAVE_D_TYPE
                if (dir_entry->d_type == DT_REG)
#endif
                    entries += 1;
            }
        }
    }

    if (!real_path.empty())
    {
        real_path_output += ("->" + real_path);
    }
    std::cout << get_permission(status) << ". " << entries << " " << get_usrname(status) << " " << get_grpname(status) << " " << status.st_size << " " << get_formatted_date_time(status) << " " << file << real_path_output << std::endl;
}

int main(int argc, char *argv[])
{
    std::string file = ".";
    int no_of_blocks = 0;
    struct dirent *dir_entry;

    if (argc == 2)
    {
        char *filename = argv[1];
        if (filename[0] == '/')
        {
            file = std::string(filename);
        }
        else
        {
            file = file + std::string("/") + std::string(filename);
        }
    }

    DIR *dir = opendir(file.c_str());

    if (dir == NULL)
    {
        struct stat file_status;
        if (lstat(file.c_str(), &file_status) < 0)
        {
            std::cout << file << std::endl;
            std::cout << "Can not open the directory" << std::endl;
            return EXIT_FAILURE;
        }
        print_output(file_status, file);
        return EXIT_SUCCESS;
    }

    std::map<std::string, struct stat> file_details;
    std::map<std::string, struct stat>::iterator itr;

    while ((dir_entry = readdir(dir)) != NULL)
    {
        std::string dir_filename(dir_entry->d_name);
        if (!dir_filename.empty() && dir_filename.at(0) == '.')
        {
            continue;
        }

        struct stat dir_stat;
        std::string file_path = file + "/" + dir_filename;

        if (lstat(file_path.c_str(), &dir_stat) < 0)
        {
            std::cout << "stat of " << file << " failed" << std::endl;
            return EXIT_FAILURE;
        }

        file_details[dir_filename] = dir_stat;
        no_of_blocks += dir_stat.st_blocks;
    }

    no_of_blocks /= 2;
    std::cout << "total " << no_of_blocks << std::endl;

    for (itr = file_details.begin(); itr != file_details.end(); ++itr)
    {
        print_output(itr->second, itr->first);
    }

    if (dir)
    {
        closedir(dir);
    }
    return EXIT_SUCCESS;
}