#include "Core.h"
#include "GUIElement.h"
#include "Input.h"

bool GUIElement::TestMouse()
{
    MouseState mouseState = Input::Get().GetMouseState();
    int mouseX = mouseState.x;
    int mouseY = WINDOW_HEIGHT - mouseState.y;

    int rectLeft = (int)(position.x * WINDOW_WIDTH/2) + WINDOW_WIDTH / 2;
    int rectBottom = (int)(position.y * WINDOW_HEIGHT / 2) + WINDOW_HEIGHT / 2;
    int rectRight = rectLeft + (int)(size.x * WINDOW_WIDTH / 2);
    int rectTop = rectBottom + (int)(size.y * WINDOW_HEIGHT / 2);

    return 
        (mouseX > rectLeft && mouseX < rectRight) && 
        (mouseY > rectBottom && mouseY < rectTop);
}
