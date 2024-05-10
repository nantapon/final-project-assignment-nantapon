<script>
    import * as Card from "$lib/components/ui/card/index";
    import * as Accordion from "$lib/components/ui/accordion/index";
    import { mode } from 'mode-watcher';
</script>

<Card.Root>
    <Card.Header>
        <Card.Title>Help</Card.Title>
    </Card.Header>
    <Card.Content class="space-y-2">
        <Accordion.Root class="w-[350px]" value={["setup"]} multiple>
            <Accordion.Item value="setup">
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

            <Accordion.Item value="state loss model">
                <Accordion.Trigger>State loss model</Accordion.Trigger>
                <Accordion.Content>
                    <article class="prose text-muted-foreground">
                        <p>
                            (From tc-netem manpage)
                            adds packet losses according to the 4-state Markov
                            using the transition probabilities as input
                            parameters. The parameter p13 is mandatory and if
                            used alone corresponds to the Bernoulli model. The
                            optional parameters allows to extend the model to
                            2-state (p31), 3-state (p23 and p32) and 4-state
                            (p14). State 1 corresponds to good reception,
                            State 4 to independent losses, State 3 to burst
                            losses and State 2 to good reception within a burst.
                        </p>
                        <img src={$mode === 'dark' ? "state_loss_dark.png" : "state_loss.png"} alt="state loss model" />
                    </article>
                </Accordion.Content>
            </Accordion.Item>

            <Accordion.Item value="about">
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
