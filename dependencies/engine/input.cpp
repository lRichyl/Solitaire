#include "input.h"
#include <queue>
#include <stdio.h>

static std::queue<Event> events;

static MousePosition mouseInfo;

bool GetNextEvent(Event *event){
     // Event e = {};
     if(events.size() == 0) return false;

     Event e = events.front();
     event->key = e.key;
     event->action = e.action;
     events.pop();
     return true;

}
bool IsKeyPressed(Window *window, int key){
     int state = glfwGetKey(window->GLFWInstance, key);
     return state == GLFW_PRESS;
    
}

void PollKeyboardEvents(GLFWwindow* window, int key, int scancode, int action, int mods)
{
     Event e = {key , action};
     events.push(e);
}

void CursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	mouseInfo.x = xpos;
	mouseInfo.y = ypos;
}

//TODO: Set here the different callbacks to glfw and change the name of the function.
void SetKeyboardCallback(Window *window){
	glfwSetKeyCallback(window->GLFWInstance, PollKeyboardEvents);
}

void SetCursorCallback(Window *window){
	
	glfwSetCursorPosCallback(window->GLFWInstance, CursorCallback);
}

MousePosition GetMousePosition(Window *window){
	MousePosition result;
	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window->GLFWInstance, &windowWidth, &windowHeight);
	
	double xBufferToWindowFactor = (double)window->internalWidth / (float)windowWidth; 
	double yBufferToWindowFactor = (double)window->internalHeight / (float)windowHeight;
	
	result.x = mouseInfo.x * xBufferToWindowFactor;
	result.y = mouseInfo.y * yBufferToWindowFactor;
	
	result.y = window->internalHeight - result.y;
	// printf("%f , %f , %f\n", xBufferToWindowFactor, result.x, result.y);
	
	return result;
}
void PrintEvents(){
     for(int i = 0; i < events.size(); i++){
          Event e = events.front();
          printf("%d\n", e.key);
          printf("%d\n", e.action);
          printf("\n\n");
          events.pop();
     }
}
