typedef struct
{
    float cutting_speed_mm_sec;
    float travel_speed_mm_sec;
    float y_offset_mm;
} control_panel_params_t;

void control_panel_task(void* pvParameters);