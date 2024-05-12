<script>
    import BasicStatus from "./BasicStatus.svelte";
    import AdvancedStatus from "./AdvancedStatus.svelte";

    import * as Card from "$lib/components/ui/card/index";
    import { Label } from "$lib/components/ui/label/index";
    import { Button } from "$lib/components/ui/button";
    import { Switch } from "$lib/components/ui/switch";
    import { Checkbox } from "$lib/components/ui/checkbox";
    import { blur } from "svelte/transition";

    import { isEqual } from "lodash-es";
    import { saveStatus, status, profiles, saveProfiles } from "./netsim";

    let isAdvanced = false;
    let default_profile = {
        delay_unit: "ms",
        bandwidth_unit: "Mbps"
    };
    $: profile = $status?.profile ? $status?.profile : default_profile;

    function save_status()
    {
        saveStatus($status);
        if (!isEqual($profiles[profile.name], profile)) {
            $profiles[profile.name] = {...profile};
            saveProfiles($profiles);
        }
        $status = $status;
    }

    function toggleSwitch(e)
    {
        $status.enabled = e.detail.checked;
        save_status();
    }

</script>

<Card.Root>
    <Card.Header>
        <Card.Title>Status</Card.Title>
        <Card.Description>
            Profile : {profile.name ? profile?.name : ""}
        </Card.Description>
    </Card.Header>
    <Card.Content class="space-y-2">
        <div class="flex justify-between">
            <div class="flex items-center space-x-2">
                <Switch checked={$status.enabled} on:checkedChange={toggleSwitch} /><Label>Enable</Label>
            </div>
            <div class="flex items-center space-x-2">
                <Checkbox bind:checked={isAdvanced} /><Label>Advanced</Label>
            </div>
        </div>

        {#if isAdvanced}
                <AdvancedStatus/>
        
        {:else}
                <BasicStatus/>
        
        {/if}
        
    </Card.Content>
    <Card.Footer class="flex justify-between">
        <Button on:click={save_status}>{$status.enabled ? "Save & Apply" : "Save"}</Button>
    </Card.Footer>
</Card.Root>
