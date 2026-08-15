interface Props72 { value: number; label: string }
export const View72 = (p: Props72) => (
  <section data-i="72">
    <h2>{p.label}</h2>
    {p.value > 72 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
