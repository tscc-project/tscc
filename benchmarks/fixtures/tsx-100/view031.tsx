interface Props31 { value: number; label: string }
export const View31 = (p: Props31) => (
  <section data-i="31">
    <h2>{p.label}</h2>
    {p.value > 31 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
