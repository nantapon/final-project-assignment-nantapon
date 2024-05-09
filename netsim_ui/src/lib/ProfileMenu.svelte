<script>
  import {
    EllipsisVertical,
    FilePlus,
    Files,
    FilePen,
    FileX,
    Check,
  } from "lucide-svelte";
  import * as DropdownMenu from "$lib/components/ui/dropdown-menu";
  import { createEventDispatcher } from "svelte";

  export let name;
  let dispatch = createEventDispatcher();
</script>

<DropdownMenu.Root>
  <DropdownMenu.Trigger><EllipsisVertical /></DropdownMenu.Trigger>
  <DropdownMenu.Content>
    <DropdownMenu.Group>
      <DropdownMenu.Item on:click={(e) => dispatch("setCurrentProfile")}
        ><Check class="pr-2" />Set Current Profile</DropdownMenu.Item
      >
      <DropdownMenu.Separator />
      <DropdownMenu.Item on:click={(e) => dispatch("newProfile")}><FilePlus class="pr-2" />New...</DropdownMenu.Item>
      <DropdownMenu.Item on:click={(e) => dispatch("cloneProfile")}><Files class="pr-2" />Clone...</DropdownMenu.Item>
      {#if name === "default"}
      <DropdownMenu.Item disabled><FilePen class="pr-2" />Rename...</DropdownMenu.Item>
      {:else}
      <DropdownMenu.Item on:click={(e) => dispatch("renameProfile")}><FilePen class="pr-2" />Rename...</DropdownMenu.Item>
      {/if}
      {#if name === "default"}
      <DropdownMenu.Item disabled><FileX class="pr-2" />Delete...</DropdownMenu.Item>
      {:else}
      <DropdownMenu.Item on:click={(e) => dispatch("deleteProfile")}><FileX class="pr-2" />Delete...</DropdownMenu.Item>
        {/if}
    </DropdownMenu.Group>
  </DropdownMenu.Content>
</DropdownMenu.Root>
