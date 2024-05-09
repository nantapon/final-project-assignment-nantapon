<script>
    import {status} from "./netsim";

    import { Input } from "$lib/components/ui/input";
    import { Label } from "$lib/components/ui/label";
    import { Textarea } from "$lib/components/ui/textarea";
    import * as Select from "$lib/components/ui/select/index";

    $: profile = $status?.profile ? $status?.profile : {};
</script>

<div class="space-y-1">
    <Label for="loss">Loss:</Label>
    <div class="flex items-center">
        <Input type="number" id="loss" bind:value={profile.loss} class="mr-3"></Input>%
    </div>
</div>
<div class="space-y-1">
    <Label>Delay:</Label>
    <div class="flex items-center space-x-2">
        <Input type="number" bind:value={profile.delay}></Input>
        <Select.Root bind:selected={profile.delay_unit_ui}>
            <Select.Trigger class="w-[120px]">
                <Select.Value />
            </Select.Trigger>
            <Select.Content>
                <Select.Item value="us" label="us"/>
                <Select.Item value="ms" label="ms"/>
                <Select.Item value="s" label="s"/>
            </Select.Content>
        </Select.Root>
    </div>
</div>
<div class="space-y-1">
    <Label>Bandwidth:</Label>
    <div class="flex items-center space-x-2">
        <Input type="number" bind:value={profile.bandwidth}/>
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
<div class="space-y-1">
    <Label>Description:</Label>
    <Textarea bind:value={profile.description} />
</div>
