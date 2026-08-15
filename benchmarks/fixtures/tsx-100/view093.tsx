interface Props93 { value: number; label: string }
export const View93 = (p: Props93) => (
  <section data-i="93">
    <h2>{p.label}</h2>
    {p.value > 93 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
