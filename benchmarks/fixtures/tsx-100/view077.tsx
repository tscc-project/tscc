interface Props77 { value: number; label: string }
export const View77 = (p: Props77) => (
  <section data-i="77">
    <h2>{p.label}</h2>
    {p.value > 77 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
