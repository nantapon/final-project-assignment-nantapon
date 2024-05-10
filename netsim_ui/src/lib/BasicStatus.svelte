<script>
    import { status } from "./netsim";

    import { Checkbox } from "$lib/components/ui/checkbox";
    import { Input } from "$lib/components/ui/input";
    import { Label } from "$lib/components/ui/label";
    import { Textarea } from "$lib/components/ui/textarea";
    import * as Select from "$lib/components/ui/select/index";

    $: profile = $status?.profile ? $status?.profile : {};
</script>

<div class="space-y-1">
    <div class="flex items-center space-x-2">
        <Checkbox bind:checked={profile.loss_enabled} />
        <Label for="loss">Loss:</Label>
    </div>
    <div class="flex items-center">
        <Input
            disabled={!profile.loss_enabled}
            type="number"
            id="loss"
            bind:value={profile.loss}
            class="mr-3"
            min="0"
            max="100"
        ></Input>%
    </div>
</div>
<div class="space-y-1">
    <div class="flex items-center space-x-2">
        <Checkbox bind:checked={profile.delay_enabled} />
        <Label>Delay:</Label>
    </div>
    <div class="flex items-center space-x-2">
        <Input
            disabled={!profile.delay_enabled}
            type="number"
            bind:value={profile.delay}
            min="0"
        ></Input>
        <Select.Root
            bind:selected={profile.delay_unit_ui}
            disabled={!profile.delay_enabled}
        >
            <Select.Trigger class="w-[120px]">
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
    <div class="flex items-center space-x-2">
        <Checkbox bind:checked={profile.bandwidth_enabled} />
        <Label>Bandwidth:</Label>
    </div>
    <div class="flex items-center space-x-2">
        <Input
            disabled={!profile.bandwidth_enabled}
            type="number"
            bind:value={profile.bandwidth}
        />
        <Select.Root
            bind:selected={profile.bandwidth_unit_ui}
            disabled={!profile.bandwidth_enabled}
        >
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
<div class="space-y-1">
    <Label>Description:</Label>
    <Textarea bind:value={profile.description} />
</div>
