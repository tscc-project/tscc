interface Props48 { value: number; label: string }
export const View48 = (p: Props48) => (
  <section data-i="48">
    <h2>{p.label}</h2>
    {p.value > 48 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
