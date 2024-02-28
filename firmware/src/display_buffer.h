#pragma once

#include <TFT_eSPI.h>

#define USE_DISPLAY_BUFFER 1

#define DISPLAY_BUFFER_DEBUG_SCREEN_DRAW 1
#ifdef DISPLAY_BUFFER_DEBUG_SCREEN_DRAW
#define DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_X 50
#define DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_Y 140
#define DISPLAY_BUFFER_DEBUG_SCREEN_RECT_W 140
#define DISPLAY_BUFFER_DEBUG_SCREEN_RECT_H 25
#define DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_SIZE 4
#endif

#define DISPLAY_BUFFER_MAX_SUSPENDABLE_TASKS 20 // Maximum number of tasks that can be suspended and resumed to allow for display drawing high performance

#define DISPLAY_BUFFER_DRAW_DELAY 100                  // TFT Draw task delay in ms. NOT USING FPS because I need first to ensure that there are no performance issues.
                                                       //  TARGET FPS will be implemented later but I expect having a target FPS it will loads more the MCU when under load.
#define DRAW_SEMAPHORE_TIMEOUT 2000 / portTICK_RATE_MS // The timeout for the draw semaphore

class DisplayBuffer
{
public:
    /**
     * Just the Constructor
     */
    DisplayBuffer()
    {

        // Init the TFT

        tft_.begin();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        //tft_.invertDisplay(1);
        tft_.setRotation(SK_DISPLAY_ROTATION);
        tft_.fillScreen(TFT_BLACK);

        // Init the buffer
        bufferSpr_.createSprite(TFT_WIDTH, TFT_HEIGHT);
        bufferSpr_.setColorDepth(16);
        bufferInitCompleted = true;

        // Init the semaphore
        tftBufferXSemaphore = xSemaphoreCreateMutex();

        // Init the TFT task
        tftDebugDrawCount = 0;
        lastTftDrawTimeMs = millis();
        xTaskCreate(DisplayBuffer::tftTask, "displayBufferToTftTask", 1024 * 2, NULL, ESP_TASK_MAIN_PRIO, NULL);
    }

    /**
     * Get the instance of the DisplayBuffer
     * @return DisplayBuffer* - The instance of the DisplayBuffer
     */
    static DisplayBuffer *getInstance()
    {
        if (DisplayBuffer::instance_ == nullptr)
        {
            DisplayBuffer::instance_ = new DisplayBuffer();
        }
        return DisplayBuffer::instance_;
    }

    void setLogger(Logger *logger)
    {
        logger_ = logger;
        /*if(logger_ != nullptr){
            logger_->log("Display buffer logger enabled");
        }*/
    }

    /**
     * Start a draw transaction, this MUST be called before any (mass) drawing operation. It will prevent the TFT from being drawn to while the transaction is active.
     * By calling startDrawTransaction() ... DRAW OPS....endDrawTransaction() you are guaranteed that the TFT will not be drawn with partial graphics on it.
     */
    void startDrawTransaction()
    {
        xSemaphoreTake(tftBufferXSemaphore, DRAW_SEMAPHORE_TIMEOUT);
    }

    /**
     * End a draw transaction, this MUST be called after any (mass) drawing operation. It will allow the TFT to be drawn to again or it will allow other tasks to perform tft operations.
     */
    void endDrawTransaction()
    {
        xSemaphoreGive(tftBufferXSemaphore);
    }

    /**
     * Get the TFT_eSprite buffer, this is the buffer that should be used for drawing operations.
     * @return TFT_eSprite* - The TFT_eSprite buffer
     */
    TFT_eSprite *getTftBuffer()
    {
        if (!bufferInitCompleted)
        {
            return nullptr;
        }
        TFT_eSprite *localBuffer = nullptr;
        return &bufferSpr_;
    }

    /**
     * Register a task to be suspended and resumed when the TFT is being drawn to. This is useful for high performance drawing operations.
     * Unregister tasks is not currently supported, please implement if needed.
     * @param task - The task to be suspended and resumed
     * @return bool - True if the task was registered, false if the maximum number of tasks has been reached
     */
    bool registerSuspendableTask(TaskHandle_t task)
    {
        if (suspendableTaskCount >= DISPLAY_BUFFER_MAX_SUSPENDABLE_TASKS)
        {
            return false;
        }
        suspendableTask[suspendableTaskCount] = task;
        suspendableTaskCount++;
        return true;
    }

private:
    /* The logger */
    Logger *logger_ = nullptr;

    /* The TFT instance */
    TFT_eSPI tft_ = TFT_eSPI();

    /* The real buffer */
    TFT_eSprite bufferSpr_ = TFT_eSprite(&tft_);

    /* The semaphore to control access to the buffer */
    SemaphoreHandle_t tftBufferXSemaphore = NULL;

    /* The tasks that will be suspended and resumed when the TFT is being drawn to and its counter */
    TaskHandle_t suspendableTask[DISPLAY_BUFFER_MAX_SUSPENDABLE_TASKS];
    uint8_t suspendableTaskCount = 0;

    /* A flag to indicate if the buffer has been initialized */
    bool bufferInitCompleted = false;

protected:
    /* The instance of the DisplayBuffer to manage it as a singleton */
    inline static DisplayBuffer *instance_ = nullptr;
    /* The last time the TFT was drawn to, useful to adapt the TFT task delay based on MCU load */
    inline static long lastTftDrawTimeMs = 0;
    /* The counter of the debug draw operations */
    inline static uint8_t tftDebugDrawCount = 0;

    /**
     * Suspend all the tasks that are registered to be suspended when the TFT is being drawn to
     */
    void suspendTasks()
    {
        for (int i = 0; i < 1; i++)
        {
            vTaskSuspend(suspendableTask[i]);
        }
    }

    /**
     * Resume all the tasks that are registered to be suspended when the TFT is being drawn to
     */
    void resumeTasks()
    {
        for (int i = 0; i < 1; i++)
        {
            vTaskResume(suspendableTask[i]);
        }
    }

    /**
     * The real CORE of this class, the task that will draw the TFT buffer to the TFT screen
     * @param args - The arguments for the task
     */
    static void tftTask(void *args)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        const TickType_t xFrequency = portTICK_RATE_MS * DISPLAY_BUFFER_DRAW_DELAY; // 20ms

        TickType_t xLastWakeTime;

        while (1)
        {

            DisplayBuffer::getInstance()->startDrawTransaction();
            // DisplayBuffer::getInstance()->suspendTasks();
#ifdef DEBUG_SCREEN_DRAW
            DisplayBuffer::getInstance()->getTftBuffer()->setCursor(DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_X, DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_Y, DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_SIZE);
            DisplayBuffer::getInstance()->getTftBuffer()->fillRect(DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_X, DISPLAY_BUFFER_DEBUG_SCREEN_TEXT_Y, DISPLAY_BUFFER_DEBUG_SCREEN_RECT_W, DISPLAY_BUFFER_DEBUG_SCREEN_RECT_H, TFT_WHITE);
            DisplayBuffer::getInstance()->getTftBuffer()->setTextColor(TFT_BLACK);
            DisplayBuffer::getInstance()->getTftBuffer()->printf("Draw #%d", tftDebugDrawCount++);
#endif
            DisplayBuffer::getInstance()->getTftBuffer()->pushSprite(0, 0);
            lastTftDrawTimeMs = millis();
            DisplayBuffer::getInstance()->endDrawTransaction();
            // DisplayBuffer::getInstance()->resumeTasks();
            vTaskDelay(xFrequency);
            xLastWakeTime = xTaskGetTickCount();
        }
    }
};