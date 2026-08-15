interface Props91 { value: number; label: string }
export const View91 = (p: Props91) => (
  <section data-i="91">
    <h2>{p.label}</h2>
    {p.value > 91 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
