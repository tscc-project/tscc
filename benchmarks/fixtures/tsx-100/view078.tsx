interface Props78 { value: number; label: string }
export const View78 = (p: Props78) => (
  <section data-i="78">
    <h2>{p.label}</h2>
    {p.value > 78 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
