<script>
    import * as Dialog from "$lib/components/ui/dialog";
    import { Input } from "$lib/components/ui/input";
    import { Label } from "$lib/components/ui/label";
    import { Button } from "$lib/components/ui/button";
    import { createEventDispatcher } from "svelte";

    export let title;
    export let description;
    export let haveInput;
    export let open;
    export let error;
    export let filename;

    let dispatch = createEventDispatcher();
</script>

<Dialog.Root bind:open onOpenChange={(e)=>console.log(e)}>
    <Dialog.Content>
        <Dialog.Header>
            <Dialog.Title>{title}</Dialog.Title>
            <Dialog.Description>{description}</Dialog.Description>
        </Dialog.Header>
        {#if haveInput}
        <div class="grid gap-4 py-4">
            <div class="grid grid-cols-4 items-center gap-4">
                <Label class="text-right">Name</Label>
                <Input bind:value={filename} class="col-span-3" on:keydown={(e) => {
                    if (e.code === "Enter") {
                        dispatch("confirm", {name: filename});
                        }}}/>
            </div>
            {#if error}
                <p class="text-red-500 text-center">{error}</p>
            {/if}
        </div>
        {/if}

        <Dialog.Footer>
            <Button
                type="submit"
                on:click={(e) =>
                    dispatch("confirm", { name: filename })}
                    
                >Confirm</Button
            >
        </Dialog.Footer>
    </Dialog.Content>
</Dialog.Root>
