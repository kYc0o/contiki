

# just add components here
COMPONENTS = helloWorldComponent/helloWorldComponent.ce \
			blinkComponent/blinkComponent.ce \
			sensingComponent/sensingComponent.ce

ifndef REPOSITORY_FILE
REPOSITORY_FILE = repository.repo
endif

COMPONENTS_TO_DELETE = $(patsubst %,%.del, $(COMPONENTS))

#PEPE = maria/maria
#$(info $(PEPE))


all: $(COMPONENTS)

$(COMPONENTS): %.ce : %.c
	@cd $(firstword $(subst /, ,$@)) && $(MAKE) $(lastword $(subst /, ,$@))
	@echo $(firstword $(subst /, ,$@)) $@ >> $(REPOSITORY_FILE)

clean: $(COMPONENTS_TO_DELETE)
	rm -f $(REPOSITORY_FILE)

$(COMPONENTS_TO_DELETE): %.del : %
	rm -f $<
	rm -f $(patsubst %.ce,%.o,$<)
