<script>
    import * as Card from "$lib/components/ui/card";
    import * as Table from "$lib/components/ui/table";
    import * as Tabs from "$lib/components/ui/tabs";

    import { status, profiles, defaultProfile, saveProfiles } from "./netsim";

    import { ScrollArea } from "$lib/components/ui/scroll-area";
    import { Label } from "$lib/components/ui/label";
    import { Button } from "$lib/components/ui/button";
    import { Textarea } from "$lib/components/ui/textarea";
    import ProfileMenu from "./ProfileMenu.svelte";
    import ProfileDialog from "./ProfileDialog.svelte";

    $: current_profile = $status?.profile ? $status?.profile : {};
    $: selected_profile = current_profile ? current_profile : {};

    let open;
    let error;
    let filename;

    let title;
    let description;
    let haveInput;
    let confirm;
    let fromProfile;

    function doConfirm(e) {
        confirm(e);
    }

    function onRowClick(name, profile) {
        selected_profile = profile;
    }

    function setCurrentProfile(profile) {
        $status.profile = { ...profile };
    }

    function newProfile() {
        open = true;
        error = null;
        filename = "";
        title = "New Profile";
        description = "Create a new empty profile";
        haveInput = true;
        confirm = newProfileConfirm;
    }

    function newProfileConfirm(e) {
        if ($profiles[e.detail.name]) {
            error = `The profile '${e.detail.name}' already exists`;
            return;
        }
        open = false;
        $profiles[e.detail.name] = { ...defaultProfile, name: e.detail.name };
        saveProfiles($profiles);
        setCurrentProfile($profiles[e.detail.name]);
    }

    function cloneProfile(profile) {
        open = true;
        error = null;
        filename = "";
        title = "Clone Profile";
        description = `Clone profile from profile '${profile.name}'`;
        haveInput = true;
        confirm = cloneProfileConfirm;
        fromProfile = { ...profile };
    }

    function cloneProfileConfirm(e) {
        if ($profiles[e.detail.name]) {
            error = `The profile '${e.detail.name}' already exists`;
            return;
        }
        open = false;
        $profiles[e.detail.name] = { ...fromProfile, name: e.detail.name };
        saveProfiles($profiles);
        setCurrentProfile($profiles[e.detail.name]);
    }

    function renameProfile(profile) {
        open = true;
        error = null;
        filename = "";
        title = "Rename Profile";
        description = `Rename profile from '${profile.name}'`;
        haveInput = true;
        confirm = renameProfileConfirm;
        fromProfile = { ...profile };
    }

    function renameProfileConfirm(e) {
        if ($profiles[e.detail.name]) {
            error = `The profile '${e.detail.name}' already exists`;
            return;
        }
        open = false;
        $profiles[e.detail.name] = { ...fromProfile, name: e.detail.name };
        delete $profiles[fromProfile.name];

        saveProfiles($profiles);
        setCurrentProfile($profiles[e.detail.name]);
    }

    function deleteProfile(profile) {
        open = true;
        error = null;
        filename = "";
        title = "Delete Profile";
        description = `Delete profile '${profile.name}'?`;
        haveInput = false;
        confirm = deleteProfileConfirm;
        fromProfile = { ...profile };
    }

    function deleteProfileConfirm(e) {
        open = false;
        delete $profiles[fromProfile.name];
        $profiles = {...$profiles};

        saveProfiles($profiles);
        setCurrentProfile($profiles["default"]);
    }
</script>

<Card.Root>
    <Card.Header>
        <Card.Title>Profiles</Card.Title>
        <Card.Description>
            Current profile: {current_profile.name}
        </Card.Description>
    </Card.Header>
    <Card.Content class="space-y-2">
        <div class="space-y-1">
            <Label>Available Profiles:</Label>
            <ScrollArea class="h-[300px] border">
                <Table.Root>
                    <Table.Body>
                        {#each Object.entries($profiles) as [name, profile]}
                            {#if selected_profile.name == name}
                                <Table.Row
                                    class="bg-muted"
                                    on:click={(e) => onRowClick(name, profile)}
                                >
                                    <Table.Cell>{name}</Table.Cell>
                                    <Table.Cell class="w-[20px]">
                                        <ProfileMenu
                                            bind:name
                                            on:setCurrentProfile={(e) =>
                                                setCurrentProfile(profile)}
                                            on:newProfile={(e) => newProfile()}
                                            on:cloneProfile={(e) =>
                                                cloneProfile(profile)}
                                            on:renameProfile={(e) =>
                                                renameProfile(profile)}
                                            on:deleteProfile={(e) =>
                                                deleteProfile(profile)}
                                        />
                                    </Table.Cell>
                                </Table.Row>
                            {:else}
                                <Table.Row
                                    on:click={(e) => onRowClick(name, profile)}
                                >
                                    <Table.Cell>{name}</Table.Cell>
                                    <Table.Cell class="w-[20px]">
                                        <ProfileMenu
                                            bind:name
                                            on:setCurrentProfile={(e) =>
                                                setCurrentProfile(profile)}
                                            on:newProfile={(e) => newProfile()}
                                            on:cloneProfile={(e) =>
                                                cloneProfile(profile)}
                                            on:renameProfile={(e) =>
                                                renameProfile(profile)}
                                            on:deleteProfile={(e) =>
                                                deleteProfile(profile)}
                                        />
                                    </Table.Cell>
                                </Table.Row>
                            {/if}
                        {/each}
                    </Table.Body>
                </Table.Root>
            </ScrollArea>
            {#if selected_profile.name}
                <Button on:click={(e) => setCurrentProfile(selected_profile)}
                    >Set Current Profile</Button
                >
            {:else}
                <Button disabled>Set Current Profile</Button>
            {/if}
        </div>

        <Tabs.Root value="description" class="pt-4">
            <Tabs.List class="grid w-full grid-cols-2">
                <Tabs.Trigger value="description">Description</Tabs.Trigger>
                <Tabs.Trigger value="details">Details</Tabs.Trigger>
            </Tabs.List>
            <Tabs.Content value="description">
                <Textarea
                    readonly
                    bind:value={selected_profile.description}
                    class="h-[150px]"
                />
            </Tabs.Content>
            <Tabs.Content value="details">
                <Textarea
                    readonly
                    value={JSON.stringify(selected_profile, null, 2)}
                    class="h-[150px]"
                ></Textarea>
            </Tabs.Content>
        </Tabs.Root>
    </Card.Content>
</Card.Root>

<ProfileDialog
    bind:title
    bind:description
    bind:haveInput
    bind:filename
    bind:open
    bind:error
    on:confirm={doConfirm}
/>
