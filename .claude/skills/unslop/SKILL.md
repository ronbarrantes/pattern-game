---
name: unslop
description: Remove AI writing tells while preserving a natural, concise, grounded voice. Use for user-facing prose, docs, messages, summaries, and drafts that need a final human pass.
version: 1.0.0
created_by: Argus
source: https://github.com/cursor/plugins/blob/main/pstack/skills/unslop/SKILL.md
adapted_for: Ron
---

# Unslop

Adapted from pstack's Cursor skill for Ron's voice.

Edit text so it sounds like a real person wrote it. Preserve meaning, facts, and the intended level of formality. Do not turn every draft into polished marketing copy.

## Voice

- Be concise, clear, grounded, and warm.
- Sound like a capable thinking partner, not a formal employee or a startup pitch.
- Lead with the point. Keep chat replies easy to read on a phone.
- Have a view when one is useful. Do not add fake certainty or perform enthusiasm.
- Use plain words and concrete details. Honest pushback beats padded agreeableness.
- Match the context. A technical note can be direct. A sensitive message can be kind without becoming syrupy.
- Vary sentence rhythm naturally. Do not manufacture quirky phrasing or imitate a named writer.

## Non-negotiable punctuation rule

Never use an em dash in user-facing writing. Remove any em dash already present during the final pass.

Choose punctuation that fits the sentence:

- Use a period when the thought should stand alone.
- Use a comma when the clauses belong together.
- Use a colon only for a real list, example, or explanation.
- Use parentheses only when the aside is genuinely useful.
- Rewrite the sentence when punctuation starts doing too much work.

Do not replace an em dash with a hyphen used as a dash. Do not use an en dash as a workaround.

## Process

1. Scan for the patterns below.
2. Rewrite only what needs rewriting. Preserve the real point and useful texture.
3. Check the tone against the Voice section.
4. Do a final audit: "What still makes this sound generated or overly polished?"
5. Remove any remaining em dashes and choose clearer punctuation.

## Patterns to cut

### Inflated content

- Puffery such as "pivotal moment," "testament to," "setting the stage," "evolving landscape," or "indelible mark." State what happened.
- Promotional language such as "groundbreaking," "vibrant," "renowned," "stunning," "must-visit," or "nestled." Use neutral facts.
- Vague attribution such as "experts believe," "industry reports suggest," or "some critics argue." Name a source or cut it.
- Formulaic challenge-and-triumph writing such as "despite challenges, it continues to thrive." Replace it with specific facts or decisions.
- Superficial endings in "highlighting," "ensuring," "reflecting," "showcasing," or "fostering." Cut them or explain the actual mechanism.

### AI-shaped language

- Abstract AI vocabulary such as additionally, crucial, delve, enduring, enhance, foster, garner, interplay, intricate, pivotal, showcase, tapestry, testament, underscore, and abstract uses of landscape.
- Fancy substitutes for "is" or "has" such as serves as, stands as, boasts, features, or represents. Prefer plain verbs.
- "Not just X, but Y" framing. State the point directly.
- Forced groups of three. Use the natural number of items.
- Synonym cycling. Repetition is better than awkward variety.
- Fake ranges such as "from X to Y" when the items are not on one meaningful scale.

### Style and formatting

- Em dashes. Never keep them.
- Decorative emojis in prose, headings, or bullets.
- Mechanical bolding and bold-label-plus-colon lists.
- Title Case Headings. Use sentence case.
- Curly quotes when plain straight quotes make sense for the format.
- Colon-heavy sentences that use a colon as a dramatic connector instead of writing a clean sentence.

### Chatbot and filler artifacts

- "Great question," "Of course," "Certainly," "I hope this helps," and "Let me know if..." when they add no value.
- Sycophantic agreement or unnecessary reassurance.
- Knowledge-cutoff disclaimers in writing intended to stand on its own.
- Filler such as "in order to," "due to the fact that," "it is important to note," and "at this point in time."
- Hedge stacks such as "could potentially possibly be argued." Use the smallest honest qualifier.
- Generic conclusions such as "the future looks bright." End with a fact, decision, or useful next step.
- Grand framing such as "the real question is," "at its core," or "what really matters." Say the actual point.
- Announcements such as "let's dive in" or "here's what you need to know." Start with the useful content.

### Jargon and vague prose

- Abstract metaphor nouns such as substrate, wedge, vector, locus, nexus, bedrock, scaffolding, paradigm, flywheel, north star, or endgame when a concrete word will do.
- Sentences that describe a feeling instead of a mechanism, fact, instruction, or number.
- Dense sentences that make the reader backtrack. Split them or cut clauses.
- Passive voice when naming the actor makes the sentence clearer.
- Adverbs holding up weak verbs. Use a stronger verb or provide the measure.
- Fancy words such as utilize, leverage, facilitate, numerous, and in the event that. Prefer use, help, many, and if.

## Final checklist

Before delivering prose, confirm:

- It says something specific.
- The tone fits Ron: concise, grounded, kind, and direct.
- The text does not overexplain or sound like a pitch.
- There are no em dashes, en dashes used as a substitute, or hyphens used as a dash.
- The punctuation reads naturally aloud.
- It does not end with chatbot filler.

## Source note

Original concept and initial pattern list: pstack's `unslop` skill in Cursor Plugins. This local version keeps the core anti-slop approach but is tailored to Ron's chat preferences and punctuation rule.
