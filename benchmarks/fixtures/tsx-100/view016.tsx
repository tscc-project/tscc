interface Props16 { value: number; label: string }
export const View16 = (p: Props16) => (
  <section data-i="16">
    <h2>{p.label}</h2>
    {p.value > 16 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
