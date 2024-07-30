## RenderPass API design

nu_create_pass(&ctx, &info, &pass);

nu_begin_pass(&ctx, &info, &pass);

nu_end_pass(&ctx, &pass, &info);

nu_render(&ctx);

nu_delete_pass(&ctx, &pass);
