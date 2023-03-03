#include "core.h"
#include "Vertex.h"

bool VertexTextured::operator==(const VertexTextured& other)
{
    if ((this->pos == other.pos) &&
        (this->nor == other.nor) &&
        (this->tan == other.tan) &&
        (this->bitan == other.bitan) &&
        (this->uv == other.uv))
    {
        return true;
    }
    return false;
}

bool VertexTextured::IsSamePNU(const VertexTextured& other)
{
    if (this->pos.x == other.pos.x && this->pos.y == other.pos.x && this->pos.z == other.pos.z)
    {
        return true;
    }
    return false;

    //return ((this->pos == other.pos));// && (this->nor == other.nor));// && (this->uv == other.uv));
    // return false;
}

bool VertexRigged::operator==(const VertexRigged& other)
{
    if ((this->pos == other.pos) &&
        (this->nor == other.nor) &&
        (this->tan == other.tan) &&
        (this->bitan == other.bitan) &&
        (this->uv == other.uv) &&
        (&this->bones[0] == &other.bones[0]) && // Comparison in accordance with: https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5056?view=msvc-170
        (&this->weights[0] == &other.weights[0])) 
    {
        return true;
    }
    return false;
}

