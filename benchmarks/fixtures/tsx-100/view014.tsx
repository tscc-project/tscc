interface Props14 { value: number; label: string }
export const View14 = (p: Props14) => (
  <section data-i="14">
    <h2>{p.label}</h2>
    {p.value > 14 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
