idf_component_register(SRCS "src/main.c"
                            "src/wifi_manager.c"
                            "src/telegram_client.c"
                            "src/display_manager.c"
                            "src/graphic.c"
                            "src/fonts/font.c"
                            "src/sprites/eye.c"
                            "src/sprites/bubble.c"
                            "src/sprites/mouth.c"
                            "src/sprites/whiskers.c"
                            "src/sprites/flush.c"
                            "src/my_queue.c"
                            "src/animation/animation.c"
                            "src/system_events.c"
                            "src/app.c"
                            "src/sprites/sprite.c"
                            "src/animation/cat_animation.c"
                            "src/buttons.c"
                            "src/menu.c"
                            "src/fonts/PressStart2P13.c"
                            "src/fonts/PressStart2P14.c"
                            "src/sprites/menu_sprites.c"
                            "src/fonts/Tiny16.c"
                            "src/sprites/hud.c"
                            "src/pet.c"
                            "src/max17048.c"
                    INCLUDE_DIRS "include" "." 
                    PRIV_REQUIRES json 
                                  esp_wifi 
                                  nvs_flash
                                  esp_http_client
                                  esp_timer
                                  mbedtls
                                  driver
                                  esp_lcd
                                  lwip
                                  esp_driver_ledc)




