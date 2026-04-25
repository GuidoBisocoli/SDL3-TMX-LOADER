#include "MapLayer.h"

#include "Texture.h"

MapLayer::MapLayer(tmx::Map& map, std::size_t idx, std::vector<Texture*> textures)
{
    const auto& layers = map.getLayers();
    if (layers[idx]->getType() != tmx::Layer::Type::Tile)
        std::cout << "layer " << idx << " is not a Tile layer..." << std::endl;

    else {
        const auto& layer = layers[idx]->getLayerAs<tmx::TileLayer>();
        const auto mapSize = map.getTileCount();
        const auto mapTileSize = map.getTileSize();
        const auto& tileSets = map.getTilesets();

        const auto tintColor = layer.getTintColour(); /// SDL2 0 to 255 -> SDL3 0.f to 1.f
        const SDL_FColor vertColor =
        {
            tintColor.r / 255.f,
            tintColor.g / 255.f,
            tintColor.b / 255.f,
            tintColor.a / 255.f
        };

        for (auto i = 0u; i < tileSets.size(); ++i) // for every tileset
        {
            //check tile ID to see if it falls within the current tile set
            const auto& ts = tileSets[i];
            const auto& tileIDs = layer.getTiles();

            const auto texSize = textures[i]->getSize();
            const auto tileCountX = texSize.x / mapTileSize.x;
            const auto tileCountY = texSize.y / mapTileSize.y;

            const float uNorm = static_cast<float>(mapTileSize.x) / texSize.x;
            const float vNorm = static_cast<float>(mapTileSize.y) / texSize.y;

            std::vector<SDL_Vertex> verts;
            for (auto y = 0u; y < mapSize.y; ++y) // from top left to bottom right
            {
                for (auto x = 0u; x < mapSize.x; ++x)
                {
                    const auto idx = y * mapSize.x + x; // current tile
                    if (idx < tileIDs.size() && tileIDs[idx].ID >= ts.getFirstGID()
                        && tileIDs[idx].ID < (ts.getFirstGID() + ts.getTileCount()))
                    {
                        //tex coords
                        auto idIndex = (tileIDs[idx].ID - ts.getFirstGID());
                        float u = static_cast<float>(idIndex % tileCountX); // tileset coordinates
                        float v = static_cast<float>(idIndex / tileCountX);
                        u *= mapTileSize.x; // TODO we should be using the tile set size, as this may be different from the map's grid size
                        v *= mapTileSize.y; // value in pixels

                        //normalise the UV
                        u /= textures[i]->getSize().x; // percentage of this tile in the tileset (normalized position)
                        v /= textures[i]->getSize().y;

                        //vert pos
                        const float tilePosX = static_cast<float>(x) * mapTileSize.x;
                        const float tilePosY = (static_cast<float>(y) * mapTileSize.y);

                        //push back to vert array
                        SDL_Vertex vert = { { tilePosX, tilePosY }, vertColor, {u, v} }; // top left
                        verts.emplace_back(vert);
                        vert = { { tilePosX + mapTileSize.x, tilePosY }, vertColor, {u + uNorm, v} }; // top right
                        verts.emplace_back(vert);
                        vert = { { tilePosX, tilePosY + mapTileSize.y}, vertColor, {u, v + vNorm} }; // bottom left
                        verts.emplace_back(vert);

                        vert = { { tilePosX, tilePosY + mapTileSize.y}, vertColor, {u, v + vNorm} }; // bottom left
                        verts.emplace_back(vert);
                        vert = { { tilePosX + mapTileSize.x, tilePosY }, vertColor, {u + uNorm, v} }; // top right
                        verts.emplace_back(vert);
                        vert = { { tilePosX + mapTileSize.x, tilePosY + mapTileSize.y }, vertColor, {u + uNorm, v + vNorm} }; // bottom right
                        verts.emplace_back(vert);

                        doFlips(tileIDs[idx].flipFlags, &verts[verts.size() - 6].tex_coord, &verts[verts.size() - 5].tex_coord, &verts[verts.size() - 4].tex_coord, &verts[verts.size() - 3].tex_coord, &verts[verts.size() - 2].tex_coord, &verts[verts.size() - 1].tex_coord);
                    }

                }
            }

            if (!verts.empty())
            {                
                texture = *textures[i];
                fullMapVertexData.swap(verts);
            }
        }
    }
}

void MapLayer::draw(SDL_Renderer* Renderer)
{
    SDL_RenderGeometry(Renderer, texture, visibleMapVertexData.data(), static_cast<std::int32_t>(visibleMapVertexData.size()), nullptr, 0);
}

void MapLayer::update(SDL_FRect view)
{
    visibleMapVertexData.clear();

    std::vector<SDL_Vertex> verts;
    SDL_Point aux{ 0, 0 };
    SDL_Vertex vert = { };

    for (const auto& v : fullMapVertexData) {
        // SDL_FPoint to SDL_Point for SDL_PointInRect
        //aux.x = v.position.x; 
        //aux.y = v.position.y;

       // if (SDL_PointInRect(&aux, &view)) { // TO DO: this won't work (it draws with errors), I guess because the vertices have to "close", they can't have an open vertex
            vert.color = v.color;
            vert.tex_coord = v.tex_coord;
            vert.position = { v.position.x - view.x, v.position.y - view.y }; // scrolling

            verts.emplace_back(vert);
        //}
    }

    if (!verts.empty())
        visibleMapVertexData.swap(verts);
}

void MapLayer::flipY(SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5)
{
    //Flip Y
    SDL_FPoint tmp0 = *v2;
    v2->y = v0->y;
    v3->y = v0->y;
    v0->y = tmp0.y;
    SDL_FPoint tmp2 = *v1;
    v1->y = v5->y;
    v4->y = v5->y;
    v5->y = tmp2.y;
}

void MapLayer::flipX(SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5)
{
    //Flip X
    SDL_FPoint tmp0 = *v4;
    v4->x = v0->x;
    v1->x = v0->x;
    v0->x = tmp0.x;
    SDL_FPoint tmp2 = *v2;
    v2->x = v5->x;
    v3->x = v5->x;
    v5->x = tmp2.x;
}

void MapLayer::flipD(SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5)
{
    SDL_FPoint tmp1 = *v1; // Right Bottom
    SDL_FPoint tmp2 = *v2; // Left Top

    *v1 = tmp2;
    *v2 = tmp1;
    *v3 = tmp1;
    *v4 = tmp2;
}

void MapLayer::doFlips(std::uint8_t bits, SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5)
{
    //0000 = no change
    //0100 = vertical = swap y axis
    //1000 = horizontal = swap x axis
    //1100 = horiz + vert = swap both axes = horiz+vert = rotate 180 degrees
    //0010 = diag = rotate 90 degrees right and swap x axis
    //0110 = diag+vert = rotate 270 degrees right
    //1010 = horiz+diag = rotate 90 degrees right
    //1110 = horiz+vert+diag = rotate 90 degrees right and swap y axis
    if (!(bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        !(bits & tmx::TileLayer::FlipFlag::Vertical) &&
        !(bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //Shortcircuit tests for nothing to do
        return;
    }
    else if (!(bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        (bits & tmx::TileLayer::FlipFlag::Vertical) &&
        !(bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //0100
        flipY(v0, v1, v2, v3, v4, v5);
    }
    else if ((bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        !(bits & tmx::TileLayer::FlipFlag::Vertical) &&
        !(bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //1000
        flipX(v0, v1, v2, v3, v4, v5);
    }
    else if ((bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        (bits & tmx::TileLayer::FlipFlag::Vertical) &&
        !(bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //1100
        flipY(v0, v1, v2, v3, v4, v5);
        flipX(v0, v1, v2, v3, v4, v5);
    }
    else if (!(bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        !(bits & tmx::TileLayer::FlipFlag::Vertical) &&
        (bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //0010
        flipD(v0, v1, v2, v3, v4, v5);
    }
    else if (!(bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        (bits & tmx::TileLayer::FlipFlag::Vertical) &&
        (bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //0110
        flipX(v0, v1, v2, v3, v4, v5);
        flipD(v0, v1, v2, v3, v4, v5);
    }
    else if ((bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        !(bits & tmx::TileLayer::FlipFlag::Vertical) &&
        (bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //1010
        flipY(v0, v1, v2, v3, v4, v5);
        flipD(v0, v1, v2, v3, v4, v5);
    }
    else if ((bits & tmx::TileLayer::FlipFlag::Horizontal) &&
        (bits & tmx::TileLayer::FlipFlag::Vertical) &&
        (bits & tmx::TileLayer::FlipFlag::Diagonal))
    {
        //1110
        flipY(v0, v1, v2, v3, v4, v5);
        flipX(v0, v1, v2, v3, v4, v5);
        flipD(v0, v1, v2, v3, v4, v5);
    }
}
