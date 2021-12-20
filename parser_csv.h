#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <exception>

namespace write_tuple {
	template<std::size_t Index, class ...TParams>
	struct _foreach_
	{
		static void tupleForeach_(const std::tuple<TParams...>& tuple)
		{
			// такой пересчёт необходим для выполнения callback'a над элементами в порядке их следования
			const std::size_t idx = sizeof...(TParams) - Index;
			std::cout << std::get<idx>(tuple);
			if (idx != sizeof...(TParams)) {
				std::cout << " ";
			}
			_foreach_<Index - 1, TParams...>::tupleForeach_(tuple);
		}
	};

	template<class ...TParams>
	struct _foreach_<0, TParams...>
	{
		static void tupleForeach_(const std::tuple<TParams...>& /*tuple*/) {}
	};

	template<class ...TParams>
	void write_tuple(const std::tuple<TParams...>& tuple)
	{
		_foreach_<sizeof...(TParams), TParams...>::tupleForeach_(tuple);
	}
}





template<class Ch, class Tr, typename ... Args>
std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t) {
	write_tuple::write_tuple(t);
	return os;
}





std::string cut_first(std::string* string) {
	int l = string->size();
	std::string res;
	int i = 0;
	while ((*string)[i] != ',' && i != l) {
		res.push_back((*string)[i]);
		i++;
	}
	string->erase(0, i+1);
	return res;

}

template<typename First, typename ...Args>
struct gettuple;

template<>
struct gettuple <std::string> {
	std::tuple<std::string> operator()(std::string* string) {
		return make_tuple(cut_first(string));
	}
};

template<>
struct gettuple <int> {
	std::tuple<int> operator()(std::string* string) {
		return std::make_tuple(stoi(cut_first(string)));
	}
};

template<typename ...Args>
struct gettuple<int, Args...> {
	gettuple<Args...> gettuple;
	std::tuple<int, Args...>  operator() (std::string* string) {
		std::tuple<int> tuple = std::make_tuple(stoi(cut_first(string)));
		return std::tuple_cat(tuple, gettuple(string));
	}
};

template<typename ...Args>
struct gettuple<std::string, Args...> {
	gettuple<Args...> gettuple;
	std::tuple<std::string, Args...>  operator() (std::string* string) {
		std::tuple<std::string> tuple = std::make_tuple(cut_first(string));
		return std::tuple_cat(tuple, gettuple(string));
	}
};







template<typename ...Args>
class InputIterator {
private:
	std::string string;
	gettuple<Args...> gettuple;
	std::string file_name;
	std::ifstream file;
	int line;
	
public:
	InputIterator() {
		string = "";
		end = 1;
	};
	InputIterator(std::string file_in) : file_name(file_in), line(1) {
		file.open(file_name);
		getline(file, string);
		end = 0;
	}
	InputIterator(const InputIterator<Args...>& ii) : file_name(ii.file_name), line(ii.line), end(ii.end){
		file.open(file_name);
		getline(file, string);
		for (int i = 0; i < line - 1; i++) {
			getline(file, string);
		}
	}
	~InputIterator() {
		file.close();
	}


	std::tuple <Args...> operator* () {
		std::tuple<Args...> t;
		try {
			t = gettuple(&string);
		}
		catch(std::exception &ex){
			std::cout << "Line #" << line << " from " << file_name << " is bad ";
		}
		return t;
	}

	InputIterator<Args...>& operator++() {
		if (!file.eof()) {
			getline(file, string);
			line++;
		}
		else {
			end = 1;
		}
		return *this;
	}

	bool operator!= (const InputIterator<Args...>& r) const {
		if (file_name != r.file_name) {
			return 1;
		}

		if (r.end) {
			if (end) {
				return 0;
			}
			return 1;
		}
		else {
			if (!end) {
				return 1;
			}
			if (string == r.string) {
				return 0;
			}
			return 1;
		}
	}
	bool end;
};

template<typename ... Args>
class CSVParser {
private:
	std::string file_name;
	int skip;
public:
	CSVParser(std::string file_in, int skip_lines /*skip first lines count*/) : file_name(file_in), skip(skip_lines) {

	}
	InputIterator<Args...> begin() {
		InputIterator<Args...>it(file_name);
		for (int i = 0; i < skip; i++) {
			++it;
		}

		return it;
	}
	InputIterator<Args...> end() {
		InputIterator<Args...>it(file_name);
		it.end = 1;
		return it;
	}
};


