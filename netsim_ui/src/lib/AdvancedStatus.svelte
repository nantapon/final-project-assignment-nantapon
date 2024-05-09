<script>
    import * as Accordion from "$lib/components/ui/accordion";
    import * as RadioGroup from "$lib/components/ui/radio-group";
    import * as Select from "$lib/components/ui/select/index";

    import { status } from "./netsim"

    import { Input } from "$lib/components/ui/input";
    import { Label } from "$lib/components/ui/label";
    import { Checkbox } from "$lib/components/ui/checkbox";
    import { Textarea } from "$lib/components/ui/textarea";

    $: profile = $status?.profile ? $status?.profile : {};
</script>

<Accordion.Root class="w-full" value={["loss",  "corrupt","delay", "bandwidth", "description"]} multiple>
    <Accordion.Item value="loss">
        <Accordion.Trigger>Loss</Accordion.Trigger>
        <Accordion.Content>
            <div class="space-y-1">
                <Label>Model:</Label>
                <RadioGroup.Root
                    bind:value={profile.loss_model}
                    class="flex space-between"
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
                <div class="space-y-1 mx-1">
                    <Label>Loss:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.loss} class="mr-3"/>%
                    </div>
                </div>
            {/if}

            {#if profile.loss_model === "state"}
                <div class="space-y-1 mx-1">
                    <Label>p13:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.p13} class="mr-3"/>%
                    </div>
                </div>
                <div class="space-y-1 mx-1">
                    <Label>p31:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.p31} class="mr-3"/>%
                    </div>
                </div>
                <div class="space-y-1 mx-1">
                    <Label>p32:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.p32} class="mr-3"/>%
                    </div>
                </div>
                <div class="space-y-1 mx-1">
                    <Label>p23:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.p23} class="mr-3"/>%
                    </div>
                </div>
                <div class="space-y-1 mx-1">
                    <Label>p14:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.p14} class="mr-3"/>%
                    </div>
                </div>
            {/if}

            {#if profile.loss_model === "gemodel"}
                <div class="space-y-1 mx-1">
                    <Label>p:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.p} class="mr-3"/>%
                    </div>
                </div>
                <div class="space-y-1 mx-1">
                    <Label>r:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.r} class="mr-3"/>%
                    </div>
                </div>
                <div class="space-y-1 mx-1">
                    <Label>1-h:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.h_bar} class="mr-3"/>%
                    </div>
                </div>
                <div class="space-y-1 mx-1">
                    <Label>1-k:</Label>
                    <div class="flex items-center">
                        <Input type="number" bind:value={profile.k_bar} class="mr-3"/>%
                    </div>
                </div>
            {/if}

            <div class="mt-2 flex items-center space-x-2">
                <Label>ECN:</Label>
                <Checkbox bind:checked={profile.ecn} />
            </div>
        </Accordion.Content>
    </Accordion.Item>
    <Accordion.Item value="corrupt">
        <Accordion.Trigger>Corrupt</Accordion.Trigger>
        <Accordion.Content>
            <div class="space-y-1">
                <Label>Corrupt:</Label>
                <div class="flex items-center">
                    <Input type="number" bind:value={profile.corrupt} class="mr-3"/>%
                </div>
                <Label>Correlation:</Label>
                <div class="flex items-center">
                    <Input type="number" bind:value={profile.corrupt_corr} class="mr-3"/>%
                </div>
            </div>
        </Accordion.Content>
    </Accordion.Item>
    <Accordion.Item value="delay">
        <Accordion.Trigger>Delay</Accordion.Trigger>
        <Accordion.Content>
            <div class="space-y-1">
                <Label>Delay:</Label>
                <div class="flex items-center space-x-2 mx-1">
                    <Input bind:value={profile.delay}></Input>
                    <Select.Root bind:selected={profile.delay_unit_ui}>
                        <Select.Trigger class="w-[120px]">
                            <Select.Value />
                        </Select.Trigger>
                        <Select.Content>
                            <Select.Item value="us" label="us" />
                            <Select.Item value="ms" label="ms" />
                            <Select.Item value="s" label="s" />
                        </Select.Content>
                    </Select.Root>
                </div>
            </div>
        </Accordion.Content>
    </Accordion.Item>
    <Accordion.Item value="bandwidth">
        <Accordion.Trigger>Bandwidth</Accordion.Trigger>
        <Accordion.Content>
            <div class="space-y-1">
                <Label>Bandwidth:</Label>

                <div class="flex items-center space-x-2 mx-1">
                    <Input bind:value={profile.bandwidth}></Input>
                    <Select.Root bind:selected={profile.bandwidth_unit_ui}>
                        <Select.Trigger class="w-[120px]">
                            <Select.Value />
                        </Select.Trigger>
                        <Select.Content>
                            <Select.Item value="bps" label="bit/s" />
                            <Select.Item value="kbps" label="kbit/s" />
                            <Select.Item value="Mbps" label="Mbit/s" />
                            <Select.Item value="Gbps" label="Gbit/s" />
                            <Select.Separator />
                            <Select.Item value="Bps" label="B/s" />
                            <Select.Item value="kBps" label="kB/s" />
                            <Select.Item value="MBps" label="MB/s" />
                            <Select.Item value="GBps" label="GB/s" />
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
