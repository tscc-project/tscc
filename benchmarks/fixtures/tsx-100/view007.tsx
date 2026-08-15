interface Props7 { value: number; label: string }
export const View7 = (p: Props7) => (
  <section data-i="7">
    <h2>{p.label}</h2>
    {p.value > 7 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
