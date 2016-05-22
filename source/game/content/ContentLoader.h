#pragma once

enum class ContentType
{
	Building
};

class ContentLoader
{
	friend class ContentManager;
public:
	enum Flags
	{
		HaveDatafile = 1 << 0,
		//HaveResources = 1 << 1,
		HaveTextfile = 1 << 1
	};

	ContentLoader(ContentType type, cstring id, int flags) : type(type), id(id), flags(flags), name(nullptr)
	{
		assert(id);
		assert(IS_SET(flags, HaveDatafile | HaveTextfile));
	}
	ContentLoader(ContentType type, cstring id, int flags, std::initializer_list<ContentType> const & dependency) : type(type), id(id),
		flags(flags), dependency(dependency), name(nullptr)
	{
		assert(id);
		assert(IS_SET(flags, HaveDatafile | HaveTextfile));
		assert(!this->dependency.empty());
	}
	virtual ~ContentLoader() {}

	virtual void Init() = 0;
	virtual void Cleanup() = 0;
	virtual int Load() { return 0; }
	virtual int LoadText() { return 0; }

	inline static cstring GetItemString(cstring type, const string& str)
	{
		if(str.empty())
			return type;
		else
			return Format("%s '%s'", type, str.c_str());
	}

protected:
	Tokenizer t, t2;

private:
	cstring id, name;
	ContentType type;
	vector<ContentType> dependency;
	vector<ContentLoader*> edge_from, edge_to;
	uint incoming_edges;
	int flags;
	bool marked;
};
