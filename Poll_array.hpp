#ifndef POLL_ARRAY_HPP
# define POLL_ARRAY_HPP

# include <set>
# include <vector>
# include <poll.h>

class Poll_array
{
private:
	struct pollfd array[100];
	int	len;
	std::set<int> indexes_to_remove;
	std::vector<pollfd> to_add;

public:
	Poll_array(): len(0) {}
	~Poll_array() {}

	void tag_for_removal(int index)
	{
		indexes_to_remove.insert(index);
	}

	void tag_for_addition(int fd, int read=1, int write=0)
	{
		pollfd new_fd;
		new_fd.fd = fd;
		new_fd.events = 0;
		if (read)
			new_fd.events |= POLLIN;
		if (write)
			new_fd.events |= POLLOUT;
		to_add.push_back(new_fd);
	}

	void update()
	{
		// make removals
		std::set<int>::reverse_iterator rit = indexes_to_remove.rbegin();
		while (rit != indexes_to_remove.rend())
		{
			array[*rit] = array[len - 1];
			--len;
			++rit;
		}
		indexes_to_remove.clear();

		// make insertions
		for (size_t i = 0; i < to_add.size(); i++)
		{
			array[len] = to_add[i];
			++len;
		}
		to_add.clear();
	}

	int getLen()
	{
		return len;
	}

	pollfd *getArray()
	{
		return array;
	}

	pollfd & operator[](int index)
	{
		return array[index];
	}

};

#endif