<script>    
    import "./app.css";
    import ThemeButton from "./lib/ThemeButton.svelte";
    import HelpPage from "./lib/HelpPage.svelte";
    import StatusPage from "./lib/StatusPage.svelte";
    import ProfilesPage from "./lib/ProfilesPage.svelte";
    import { getStatus, getProfiles, status, profiles } from "./lib/netsim";

    import * as Tabs from "$lib/components/ui/tabs";
    import { ScrollArea } from "$lib/components/ui/scroll-area";

    let promise = getStatus();
    promise.then((status1) => {
        $status = { ...status1 };
        // console.log($status)  
    });

    let promise2 = getProfiles();
    promise2.then((profiles1) => {
        $profiles = { ...profiles1 };
        // console.log($profiles);
    });
</script>

<main class="h-screen w-screen flex justify-center items-top p-6">
    <div class="absolute right-2 top-2">
        <ThemeButton></ThemeButton>
    </div>
    <div class="w-[400px] xl:w-full">
        {#if $status.enabled}
            <h1
                class="text-4xl font-extrabold text-center p-4 bg-gradient-to-r from-blue-500 via-teal-500 to-pink-500 bg-clip-text text-transparent"
            >
                Network simulator
            </h1>
        {:else}
            <h1 class="text-4xl font-extrabold text-center p-4">
                Network simulator
            </h1>
        {/if}
        {#await promise catch error}
            <p class="text-foreground text-center">{error.message}</p>
        {/await}
        {#await promise2 catch error}
            <p class="text-foreground text-center">{error.message}</p>
        {/await}
        <div class="w-[400px] block xl:hidden">
            <Tabs.Root value="status">
                <Tabs.List class="grid w-full grid-cols-3">
                    <Tabs.Trigger value="status">Status</Tabs.Trigger>
                    <Tabs.Trigger value="profiles">Profiles</Tabs.Trigger>
                    <Tabs.Trigger value="help">Help</Tabs.Trigger>
                </Tabs.List>
                <ScrollArea class="h-[800px]">
                    <Tabs.Content value="status">
                        <StatusPage/>
                    </Tabs.Content>
                    <Tabs.Content value="profiles">
                        <ProfilesPage/>
                    </Tabs.Content>
                    <Tabs.Content value="help">
                        <HelpPage/>
                    </Tabs.Content>
                </ScrollArea>
            </Tabs.Root>
        </div>
        <div class="w-full space-x-4 hidden justify-center xl:flex">
            <div class="w-[400px]">
                <ProfilesPage/>
            </div>
            <div class="w-[400px]">
                <StatusPage/>
            </div>
            <div class="w-[400px]">
                <HelpPage/>
            </div>
        </div>
    </div>
</main>

<style>
</style>
