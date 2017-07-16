#include "residgenerator.hpp"


#include "resid.hpp"


//
// ResIDGenerator
//


class ResIDGenerator
{
    friend unsigned int get_res_ID(const std::string&);
    friend std::string get_res_from_ID(unsigned int);

private:
    static ResIDGenerator& generator();

    ResIDGenerator();
    ResIDGenerator(const ResIDGenerator&) = delete;
    void operator=(const ResIDGenerator&) = delete;
public:
    ~ResIDGenerator();

    unsigned int get_ID(const std::string& resource);
    std::string get_resource(unsigned int ID);

private:

    std::map<std::string, unsigned int> resToID;
    std::map<unsigned int, std::string> IDtoRes;
};


ResIDGenerator& ResIDGenerator::generator()
{
    static ResIDGenerator gen;
    return gen;
}


ResIDGenerator::ResIDGenerator()
{
    for(auto& pair : RESOURCE_ID_MAP)
    {
        resToID[pair.first] = pair.second;
        IDtoRes[pair.second] = pair.first;
    }
}


ResIDGenerator::~ResIDGenerator()
{

}


unsigned int ResIDGenerator::get_ID(const std::string& resource)
{
    auto it = resToID.find(resource);
    if(it != resToID.end())
    {
        return it->second;
    }
    else
    {
        unsigned int newID = resToID.size();
        resToID[resource] = newID;
        IDtoRes[newID] = resource;
        return newID;
    }
}


std::string ResIDGenerator::get_resource(unsigned int ID)
{
    auto it = IDtoRes.find(ID);
    if(it != IDtoRes.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("unknown resource ID");
    }
}



//
// Other
//


unsigned int get_res_ID(const std::string& resource)
{
    return ResIDGenerator::generator().get_ID(resource);
}


std::string get_res_from_ID(unsigned int ID)
{
    return ResIDGenerator::generator().get_resource(ID);
}
