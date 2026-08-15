interface Props15 { value: number; label: string }
export const View15 = (p: Props15) => (
  <section data-i="15">
    <h2>{p.label}</h2>
    {p.value > 15 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
