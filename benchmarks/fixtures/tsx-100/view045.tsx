interface Props45 { value: number; label: string }
export const View45 = (p: Props45) => (
  <section data-i="45">
    <h2>{p.label}</h2>
    {p.value > 45 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
