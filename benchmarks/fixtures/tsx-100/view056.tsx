interface Props56 { value: number; label: string }
export const View56 = (p: Props56) => (
  <section data-i="56">
    <h2>{p.label}</h2>
    {p.value > 56 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
