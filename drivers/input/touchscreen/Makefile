ifeq ($(CONFIG_VIVO_TS),)
  CONFIG_VIVO_TS=y
endif

obj-$(CONFIG_VIVO_TS) += vivo_ts.o

vivo_ts-y += vts_core.o
vivo_ts-y += vts_node.o
vivo_ts-y += vts_incell.o
vivo_ts-y += vts_class.o
vivo_ts-y += vts_report.o
vivo_ts-y += vts_op.o
vivo_ts-y += vts_spi_drv.o

vivo_ts-y += $(addprefix nt-no-flash/, \
				nt36xxx.o \
				nt36xxx_ext_proc.o \
				nt36xxx_fw_update.o \
				nt36xxx_mp_ctrlram.o \
			)
vivo_ts-y += $(addprefix ft8756-no-flash/, \
				focaltech_core.o \
				focaltech_esdcheck.o \
				focaltech_ex_fun.o \
				focaltech_flash.o \
				focaltech_gesture.o \
				focaltech_spi.o \
				focaltech_test.o \
			)
vivo_ts-y += $(addprefix ili_9882n/, \
				ili9881x.o \
				ili9881x_hostdl.o \
				ili9881x_ic.o \
				ili9881x_mp.o \
				ili9881x_node.o \
				ili9881x_plat.o \
				ili9881x_spi.o \
				ili9881x_touch.o \
				vivo_v2.o \
			)
vivo_ts-y += vts_init.o
