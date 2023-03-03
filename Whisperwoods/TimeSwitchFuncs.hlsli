
// Goes from 0.0f to 1.0f depending on how long the start duration of time switch is.
float TimeSwitchStartInfluence(float time, float startDuration)
{
    return smoothstep(0.0f, startDuration, time); // Goes to 1.
}

// Goes from 1.0f to 0.0f after start duration has been reached.
float TimeSwitchEndInfluence(float time, float startDuration, float endDuration)
{
    float switchEndInfluence = smoothstep(
        startDuration,
        startDuration + endDuration,
        time
    );
    
    return 1.0 - switchEndInfluence; 
}

float TotalTimeSwitchInfluence(float time, float startDuration, float endDuration)
{
    return TimeSwitchStartInfluence(time, startDuration) * TimeSwitchEndInfluence(time, startDuration, endDuration);
}