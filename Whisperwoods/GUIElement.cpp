#include "Core.h"
#include "GUIElement.h"
#include "Input.h"

bool GUIElement::TestMouse()
{
    MouseState mouseState = Input::Get().GetMouseState();
    int mouseX = mouseState.x;
    int mouseY = WINDOW_HEIGHT - mouseState.y;

    int rectLeft = (position.x * WINDOW_WIDTH/2) + WINDOW_WIDTH / 2;
    int rectBottom = (position.y * WINDOW_HEIGHT / 2) + WINDOW_HEIGHT / 2;
    int rectRight = rectLeft + (size.x * WINDOW_WIDTH / 2);
    int rectTop = rectBottom + (size.y * WINDOW_HEIGHT / 2);

    //LOG_TRACE( "Mouse: %d %d", mouseX, mouseY );
    //LOG_TRACE( "rect: %d %d, %d %d", rectLeft, rectBottom, rectRight, rectTop );

    bool insideHor = (mouseX > rectLeft && mouseX < rectRight);
    bool insideVert = (mouseY > rectBottom && mouseY < rectTop);
    bool isInside = (insideHor && insideVert);
    //LOG_TRACE( "Inside: %d ", isInside );

    return isInside;
}
