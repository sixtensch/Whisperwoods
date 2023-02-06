#include "core.h"
#include "Vertex.h"

bool VertexTextured::operator==(const VertexTextured& other)
{
    if ((this->pos == other.pos) &&
        (this->nor == other.nor) &&
        (this->tan == other.tan) &&
        (this->bitan == other.bitan) &&
        (this->uv == other.uv) &&
        (this->padding == other.padding))
    {
        return true;
    }
    return false;
}

bool VertexRigged::operator==(const VertexRigged& other)
{
    if ((this->pos == other.pos) &&
        (this->nor == other.nor) &&
        (this->tan == other.tan) &&
        (this->bitan == other.bitan) &&
        (this->uv == other.uv) &&
        (this->padding == other.padding) &&
        (this->bones == other.bones) &&
        (this->weights == other.weights))
    {
        return true;
    }
    return false;
}
