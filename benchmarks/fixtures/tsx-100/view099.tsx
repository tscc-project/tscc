interface Props99 { value: number; label: string }
export const View99 = (p: Props99) => (
  <section data-i="99">
    <h2>{p.label}</h2>
    {p.value > 99 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
