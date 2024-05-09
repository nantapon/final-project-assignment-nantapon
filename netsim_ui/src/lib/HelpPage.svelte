<script>
    import * as Card from "$lib/components/ui/card/index";
    import * as Accordion from "$lib/components/ui/accordion/index";
</script>

<Card.Root>
    <Card.Header>
        <Card.Title>Help</Card.Title>
    </Card.Header>
    <Card.Content class="space-y-2">
        <Accordion.Root class="w-full" value={["item-1"]} multiple>
            <Accordion.Item value="item-1">
                <Accordion.Trigger>Set up</Accordion.Trigger>
                <Accordion.Content>
                    <article class="prose text-muted-foreground">
                        <p>
                            The machine under test must be set up to route the
                            message to the network simulator.
                        </p>
                        <p>
                            The example below show how to set up routing for a
                            single target ip.
                        </p>
                        <pre>
sudo ip route add &lt;target-ip&gt; via &lt;network-simlator-ip&gt; dev &lt;device&gt; 
                    </pre>
                    </article>
                </Accordion.Content>
            </Accordion.Item>
            <Accordion.Item value="item-2">
                <Accordion.Trigger>About Network Simulator</Accordion.Trigger>
                <Accordion.Content>
                    <article class="prose text-muted-foreground">
                        <p>
                            This project is submitted as a final project for
                            coursera course 'Linux Embedded System Topics and
                            Projects' offered by UC Boulder.
                        </p>
                    </article>
                </Accordion.Content>
            </Accordion.Item>
        </Accordion.Root>
    </Card.Content>
</Card.Root>
