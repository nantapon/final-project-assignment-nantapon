<script>
    import * as Accordion from "$lib/components/ui/accordion";
    import * as RadioGroup from "$lib/components/ui/radio-group";
    import * as Select from "$lib/components/ui/select/index";

    import { status } from "./netsim";

    import { Input } from "$lib/components/ui/input";
    import { Label } from "$lib/components/ui/label";
    import { Checkbox } from "$lib/components/ui/checkbox";
    import { Textarea } from "$lib/components/ui/textarea";

    $: profile = $status?.profile ? $status?.profile : {};
</script>

<Accordion.Root
    class="w-full"
    value={["loss", "delay", "bandwidth", "description"]}
    multiple
>
    <Accordion.Item value="loss">
        <Accordion.Trigger>Loss</Accordion.Trigger>
        <Accordion.Content>
            <div class="mb-2 flex items-center space-x-2">
                <Label>Enabled:</Label>
                <Checkbox bind:checked={profile.loss_enabled} />
            </div>
            <div class="space-y-1 mb-2">
                <Label>Model:</Label>
                <RadioGroup.Root
                    bind:value={profile.loss_model}
                    class="flex space-between ml-2"
                    disabled={!profile.loss_enabled}
                >
                    <div class="flex items-center space-x-2">
                        <RadioGroup.Item value="random" id="r1" />
                        <Label for="r1">random</Label>
                    </div>
                    <div class="flex items-center space-x-2">
                        <RadioGroup.Item value="state" id="r2" />
                        <Label for="r2">state</Label>
                    </div>
                    <div class="flex items-center space-x-2">
                        <RadioGroup.Item value="gemodel" id="r3" />
                        <Label for="r3">gemodel</Label>
                    </div>
                    <RadioGroup.Input name="spacing" />
                </RadioGroup.Root>
            </div>
            {#if profile.loss_model === "random"}
                <div class="space-y-1">
                    <Label>Loss:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.loss}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
            {/if}

            {#if profile.loss_model === "state"}
                <div class="space-y-1">
                    <Label>p13:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.p13}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
                <div class="space-y-1">
                    <Label>p31:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.p31}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
                <div class="space-y-1">
                    <Label>p32:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.p32}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
                <div class="space-y-1">
                    <Label>p23:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.p23}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
                <div class="space-y-1">
                    <Label>p14:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.p14}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
            {/if}

            {#if profile.loss_model === "gemodel"}
                <div class="space-y-1">
                    <Label>p:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.p}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
                <div class="space-y-1">
                    <Label>r:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.r}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
                <div class="space-y-1">
                    <Label>1-h:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.h_bar}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
                <div class="space-y-1">
                    <Label>1-k:</Label>
                    <div class="flex items-center mx-2">
                        <Input
                            type="number"
                            bind:value={profile.k_bar}
                            class="mx-2"
                            disabled={!profile.loss_enabled}
                        />%
                    </div>
                </div>
            {/if}

            <div class="mt-2 flex items-center space-x-2">
                <Label>ECN:</Label>
                <Checkbox
                    bind:checked={profile.ecn}
                    disabled={!profile.loss_enabled}
                />
            </div>
        </Accordion.Content>
    </Accordion.Item>

    <Accordion.Item value="corrupt">
        <Accordion.Trigger>Corruption</Accordion.Trigger>
        <Accordion.Content>
            <div class="mb-2 flex items-center space-x-2">
                <Label>Enabled:</Label>
                <Checkbox bind:checked={profile.corrupt_enabled} />
            </div>
            <div class="space-y-1">
                <Label>Corrupt:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.corrupt}
                        class="mx-2"
                        disabled={!profile.corrupt_enabled}
                    />%
                </div>
                <Label>Correlation:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.corrupt_corr}
                        class="mx-2"
                        disabled={!profile.corrupt_enabled}
                    />%
                </div>
            </div>
        </Accordion.Content>
    </Accordion.Item>

    <Accordion.Item value="duplicate">
        <Accordion.Trigger>Duplication</Accordion.Trigger>
        <Accordion.Content>
            <div class="mb-2 flex items-center space-x-2">
                <Label>Enabled:</Label>
                <Checkbox bind:checked={profile.duplicate_enabled} />
            </div>
            <div class="space-y-1">
                <Label>Duplicate:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.duplicate}
                        class="mx-2"
                        disabled={!profile.duplicate_enabled}
                    />%
                </div>
                <Label>Correlation:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.duplicate_corr}
                        class="mx-2"
                        disabled={!profile.duplicate_enabled}
                    />%
                </div>
            </div>
        </Accordion.Content>
    </Accordion.Item>

    <Accordion.Item value="reorder">
        <Accordion.Trigger>Reordering</Accordion.Trigger>
        <Accordion.Content>
            <div class="mb-2 flex items-center space-x-2">
                <Label>Enabled:</Label>
                <Checkbox bind:checked={profile.reorder_enabled} />
            </div>
            <div class="space-y-1">
                <Label>Duplicate:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.reorder}
                        class="mx-2"
                        disabled={!profile.reorder_enabled}
                    />%
                </div>
                <Label>Correlation:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.reorder_corr}
                        class="mx-2"
                        disabled={!profile.reorder_enabled}
                    />%
                </div>
                <Label>Gap:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.reorder_corr}
                        class="mx-2"
                        disabled={!profile.reorder_enabled}
                    />
                </div>
            </div>
        </Accordion.Content>
    </Accordion.Item>

    <Accordion.Item value="delay">
        <Accordion.Trigger>Delay</Accordion.Trigger>
        <Accordion.Content>
            <div class="mb-2 flex items-center space-x-2">
                <Label>Enabled:</Label>
                <Checkbox bind:checked={profile.delay_enabled} />
            </div>
            <div class="space-y-1">
                <Label>Delay:</Label>
                <div class="flex items-center space-x-2 mx-2">
                    <Input
                        type="number"
                        bind:value={profile.delay}
                        class="ml-2"
                        disabled={!profile.delay_enabled}
                    ></Input>
                    <Select.Root bind:selected={profile.delay_unit_ui}>
                        <Select.Trigger
                            class="w-[120px]"
                            disabled={!profile.delay_enabled}
                        >
                            <Select.Value />
                        </Select.Trigger>
                        <Select.Content>
                            <Select.Item value="ns" label="ns" />
                            <Select.Item value="us" label="us" />
                            <Select.Item value="ms" label="ms" />
                            <Select.Item value="s" label="s" />
                        </Select.Content>
                    </Select.Root>
                </div>
            </div>
            <div class="space-y-1">
                <Label>Jitter:</Label>
                <div class="flex items-center space-x-2 mx-2">
                    <Input
                        type="number"
                        bind:value={profile.jitter}
                        class="ml-2"
                        disabled={!profile.delay_enabled}
                    ></Input>
                    <Select.Root bind:selected={profile.jitter_unit_ui}>
                        <Select.Trigger
                            class="w-[120px]"
                            disabled={!profile.delay_enabled}
                        >
                            <Select.Value />
                        </Select.Trigger>
                        <Select.Content>
                            <Select.Item value="ns" label="ns" />
                            <Select.Item value="us" label="us" />
                            <Select.Item value="ms" label="ms" />
                            <Select.Item value="s" label="s" />
                        </Select.Content>
                    </Select.Root>
                </div>
            </div>
            <div class="space-y-1">
                <Label>Correlation:</Label>
                <div class="flex items-center mx-2">
                    <Input
                        type="number"
                        bind:value={profile.delay_corr}
                        class="mx-2"
                        disabled={!profile.delay_enabled}
                    />%
                </div>
            </div>
            <div class="space-y-1">
                <Label>Distribution:</Label>
                <RadioGroup.Root
                    bind:value={profile.delay_dist}
                    class="grid grid-cols-2"
                    disabled={!profile.delay_enabled}
                >
                    <div class="flex items-center space-x-2 ml-2">
                        <RadioGroup.Item value="uniform" id="r1" />
                        <Label for="r1">uniform</Label>
                    </div>
                    <div class="flex items-center space-x-2">
                        <RadioGroup.Item value="normal" id="r2" />
                        <Label for="r2">normal</Label>
                    </div>
                    <div class="flex items-center space-x-2 ml-2">
                        <RadioGroup.Item value="paleto" id="r3" />
                        <Label for="r3">paleto</Label>
                    </div>
                    <div class="flex items-center space-x-2">
                        <RadioGroup.Item value="paletonormal" id="r4" />
                        <Label for="r4">paletonormal</Label>
                    </div>
                    <RadioGroup.Input name="spacing" />
                </RadioGroup.Root>
            </div>
        </Accordion.Content>
    </Accordion.Item>
    <Accordion.Item value="bandwidth">
        <Accordion.Trigger>Bandwidth</Accordion.Trigger>
        <Accordion.Content>
            <div class="mb-2 flex items-center space-x-2">
                <Label>Enabled:</Label>
                <Checkbox bind:checked={profile.bandwidth_enabled} />
            </div>
            <div class="space-y-1">
                <Label>Bandwidth:</Label>

                <div class="flex items-center space-x-2 mx-2">
                    <Input bind:value={profile.bandwidth} class="ml-2" disabled={!profile.bandwidth_enabled}></Input>
                    <Select.Root bind:selected={profile.bandwidth_unit_ui} disabled={!profile.bandwidth_enabled}>
                        <Select.Trigger class="w-[120px]">
                            <Select.Value />
                        </Select.Trigger>
                        <Select.Content>
                            <Select.Item value="bit" label="bit/s" />
                            <Select.Item value="kbit" label="kbit/s" />
                            <Select.Item value="mbit" label="Mbit/s" />
                            <Select.Item value="gbit" label="Gbit/s" />
                            <Select.Item value="tbit" label="Tbit/s" />
                            <Select.Separator />
                            <Select.Item value="bps" label="B/s" />
                            <Select.Item value="kbps" label="kB/s" />
                            <Select.Item value="mbps" label="MB/s" />
                            <Select.Item value="gbps" label="GB/s" />
                            <Select.Item value="tbps" label="TB/s" />
                        </Select.Content>
                    </Select.Root>
                </div>
            </div>
        </Accordion.Content>
    </Accordion.Item>

    <Accordion.Item value="description">
        <Accordion.Trigger>Description</Accordion.Trigger>
        <Accordion.Content>
            <div class="space-y-1">
                <Label>Description:</Label>
                <Textarea bind:value={profile.description} />
            </div>
        </Accordion.Content>
    </Accordion.Item>
</Accordion.Root>
